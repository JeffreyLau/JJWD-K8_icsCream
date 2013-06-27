/*
 *  User level driver support for input subsystem
 *
 * Heavily based on evdev.c by Vojtech Pavlik
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Aristeu Sergio Rozanski Filho <aris@cathedrallabs.org>
 *
 * Changes/Revisions:
 *	0.3	09/04/2006 (Anssi Hannula <anssi.hannula@gmail.com>)
 *		- updated ff support for the changes in kernel interface
 *		- added MODULE_VERSION
 *	0.2	16/10/2004 (Micah Dowty <micah@navi.cx>)
 *		- added force feedback support
 *              - added UI_SET_PHYS
 *	0.1	20/06/2002
 *		- first public version
 */
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uinput.h>
#include <linux/input/mt.h>
#include "../input-compat.h"
#define GMAC_TIME_COMPARE
#ifdef GMAC_TIME_COMPARE
static struct timeval newtv;
static struct timeval oldtv;
static struct timeval first;

#define MAX_CNT 30
#define MAX_GAP_CNT 6

static unsigned int gap_time[MAX_GAP_CNT] = {0,};
static unsigned int total_times[MAX_CNT] = {0,};
static unsigned int total_time_cnt = 0;
static unsigned int total_sum_time = 0;
static unsigned int total_play_time = 0;

static unsigned int time_cnt = 0;
static unsigned int total_time = 0;
static unsigned int total_byte = 0;
#endif
#define MOUSE_INPUT_THREAD
#ifdef MOUSE_INPUT_THREAD
#include <linux/kthread.h>
#include <linux/delay.h>

#define MAX_CNT_UINPUT 100
struct task_struct * remocon_task;
static int rd_mouse_cnt[MAX_CNT_UINPUT][3];
static int rd_data_cnt = 0; wr_data_cnt = 0;
struct input_device *udev_wow;
static int remocon_thread(void *data);

#endif
static inline ssize_t uinput_inject_event(struct uinput_device *udev, const char __user *buffer, size_t count);
static int uinput_setup_device(struct uinput_device *udev, const char __user *buffer, size_t count);

static int uinput_dev_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
	struct uinput_device	*udev = input_get_drvdata(dev);

	udev->buff[udev->head].type = type;
	udev->buff[udev->head].code = code;
	udev->buff[udev->head].value = value;
	do_gettimeofday(&udev->buff[udev->head].time);
	udev->head = (udev->head + 1) % UINPUT_BUFFER_SIZE;

	wake_up_interruptible(&udev->waitq);

	return 0;
}

/* Atomically allocate an ID for the given request. Returns 0 on success. */
static int uinput_request_alloc_id(struct uinput_device *udev, struct uinput_request *request)
{
	int id;
	int err = -1;

	spin_lock(&udev->requests_lock);

	for (id = 0; id < UINPUT_NUM_REQUESTS; id++) {
		if (!udev->requests[id]) {
			request->id = id;
			udev->requests[id] = request;
			err = 0;
			break;
		}
	}

	spin_unlock(&udev->requests_lock);
	return err;
}

static struct uinput_request *uinput_request_find(struct uinput_device *udev, int id)
{
	/* Find an input request, by ID. Returns NULL if the ID isn't valid. */
	if (id >= UINPUT_NUM_REQUESTS || id < 0)
		return NULL;

	return udev->requests[id];
}

static inline int uinput_request_reserve_slot(struct uinput_device *udev, struct uinput_request *request)
{
	/* Allocate slot. If none are available right away, wait. */
	return wait_event_interruptible(udev->requests_waitq,
					!uinput_request_alloc_id(udev, request));
}

static void uinput_request_done(struct uinput_device *udev, struct uinput_request *request)
{
	/* Mark slot as available */
	udev->requests[request->id] = NULL;
	wake_up(&udev->requests_waitq);

	complete(&request->done);
}

static int uinput_request_submit(struct uinput_device *udev, struct uinput_request *request)
{
	int retval;

	retval = uinput_request_reserve_slot(udev, request);
	if (retval)
		return retval;

	retval = mutex_lock_interruptible(&udev->mutex);
	if (retval)
		return retval;

	if (udev->state != UIST_CREATED) {
		retval = -ENODEV;
		goto out;
	}

	/* Tell our userspace app about this new request by queueing an input event */
	uinput_dev_event(udev->dev, EV_UINPUT, request->code, request->id);

 out:
	mutex_unlock(&udev->mutex);
	return retval;
}

/*
 * Fail all ouitstanding requests so handlers don't wait for the userspace
 * to finish processing them.
 */
static void uinput_flush_requests(struct uinput_device *udev)
{
	struct uinput_request *request;
	int i;

	spin_lock(&udev->requests_lock);

	for (i = 0; i < UINPUT_NUM_REQUESTS; i++) {
		request = udev->requests[i];
		if (request) {
			request->retval = -ENODEV;
			uinput_request_done(udev, request);
		}
	}

	spin_unlock(&udev->requests_lock);
}

static void uinput_dev_set_gain(struct input_dev *dev, u16 gain)
{
	uinput_dev_event(dev, EV_FF, FF_GAIN, gain);
}

static void uinput_dev_set_autocenter(struct input_dev *dev, u16 magnitude)
{
	uinput_dev_event(dev, EV_FF, FF_AUTOCENTER, magnitude);
}

static int uinput_dev_playback(struct input_dev *dev, int effect_id, int value)
{
	return uinput_dev_event(dev, EV_FF, effect_id, value);
}

static int uinput_dev_upload_effect(struct input_dev *dev, struct ff_effect *effect, struct ff_effect *old)
{
	struct uinput_device *udev = input_get_drvdata(dev);
	struct uinput_request request;
	int retval;

	/*
	 * uinput driver does not currently support periodic effects with
	 * custom waveform since it does not have a way to pass buffer of
	 * samples (custom_data) to userspace. If ever there is a device
	 * supporting custom waveforms we would need to define an additional
	 * ioctl (UI_UPLOAD_SAMPLES) but for now we just bail out.
	 */
	if (effect->type == FF_PERIODIC &&
			effect->u.periodic.waveform == FF_CUSTOM)
		return -EINVAL;

	request.id = -1;
	init_completion(&request.done);
	request.code = UI_FF_UPLOAD;
	request.u.upload.effect = effect;
	request.u.upload.old = old;

	retval = uinput_request_submit(udev, &request);
	if (!retval) {
		wait_for_completion(&request.done);
		retval = request.retval;
	}

	return retval;
}

static int uinput_dev_erase_effect(struct input_dev *dev, int effect_id)
{
	struct uinput_device *udev = input_get_drvdata(dev);
	struct uinput_request request;
	int retval;

	if (!test_bit(EV_FF, dev->evbit))
		return -ENOSYS;

	request.id = -1;
	init_completion(&request.done);
	request.code = UI_FF_ERASE;
	request.u.effect_id = effect_id;

	retval = uinput_request_submit(udev, &request);
	if (!retval) {
		wait_for_completion(&request.done);
		retval = request.retval;
	}

	return retval;
}

static void uinput_destroy_device(struct uinput_device *udev)
{
	const char *name, *phys;
	struct input_dev *dev = udev->dev;
	enum uinput_state old_state = udev->state;

	udev->state = UIST_NEW_DEVICE;

	if (dev) {
		name = dev->name;
		phys = dev->phys;
		if (old_state == UIST_CREATED) {
			uinput_flush_requests(udev);
			input_unregister_device(dev);
		} else {
			input_free_device(dev);
		}
		kfree(name);
		kfree(phys);
		udev->dev = NULL;
		#ifdef MOUSE_INPUT_THREAD
		if(remocon_task){
			kthread_stop(remocon_task); 
			remocon_task = NULL;
		}
		#endif
	}
}

static int uinput_create_device(struct uinput_device *udev)
{
	struct input_dev *dev = udev->dev;
	int error;
	
	if (udev->state != UIST_SETUP_COMPLETE) {
		printk(KERN_DEBUG "%s: write device info first\n", TCC_UINPUT_NAME);
		return -EINVAL;
	}

#if 1
	if (udev->ff_effects_max) {
		error = input_ff_create(dev, udev->ff_effects_max);
		if (error)
			goto fail1;

		dev->ff->upload = uinput_dev_upload_effect;
		dev->ff->erase = uinput_dev_erase_effect;
		dev->ff->playback = uinput_dev_playback;
		dev->ff->set_gain = uinput_dev_set_gain;
		dev->ff->set_autocenter = uinput_dev_set_autocenter;
	}
#endif

	error = input_register_device(udev->dev);
	if (error)
		goto fail2;

	udev->state = UIST_CREATED;
#ifdef MOUSE_INPUT_THREAD

	//memset(rd_mouse_cnt[MAX_CNT_UINPUT][3],0x00,sizeof(rd_mouse_cnt));
	rd_data_cnt = 0;
	wr_data_cnt = 0;

	remocon_task = kthread_create(remocon_thread, NULL, "remocon_task");
	if(remocon_task)
		wake_up_process(remocon_task);
#endif

	return 0;

 fail2:	input_ff_destroy(dev);
 fail1: uinput_destroy_device(udev);
	return error;
}

static int tcc_uinput_open(struct inode *inode, struct file *file)
{
	struct uinput_device *newdev;

	newdev = kzalloc(sizeof(struct uinput_device), GFP_KERNEL);
	if (!newdev)
		return -ENOMEM;

	mutex_init(&newdev->mutex);
	spin_lock_init(&newdev->requests_lock);
	init_waitqueue_head(&newdev->requests_waitq);
	init_waitqueue_head(&newdev->waitq);
	newdev->state = UIST_NEW_DEVICE;

	file->private_data = newdev;
	nonseekable_open(inode, file);

	return 0;
}

static int uinput_validate_absbits(struct input_dev *dev)
{
	unsigned int cnt;
	int retval = 0;

	for (cnt = 0; cnt < ABS_CNT; cnt++) {
		int min, max;
		if (!test_bit(cnt, dev->absbit))
			continue;

		min = input_abs_get_min(dev, cnt);
		max = input_abs_get_max(dev, cnt);

		if ((min != 0 || max != 0) && max <= min) {
			printk(KERN_DEBUG
				"%s: invalid abs[%02x] min:%d max:%d\n",
				TCC_UINPUT_NAME, cnt,
				input_abs_get_min(dev, cnt),
				input_abs_get_max(dev, cnt));
			retval = -EINVAL;
			break;
		}

		if (input_abs_get_flat(dev, cnt) >
		    input_abs_get_max(dev, cnt) - input_abs_get_min(dev, cnt)) {
			printk(KERN_DEBUG
				"%s: abs_flat #%02x out of range: %d "
				"(min:%d/max:%d)\n",
				TCC_UINPUT_NAME, cnt,
				input_abs_get_flat(dev, cnt),
				input_abs_get_min(dev, cnt),
				input_abs_get_max(dev, cnt));
			retval = -EINVAL;
			break;
		}
	}
	return retval;
}

static int uinput_allocate_device(struct uinput_device *udev)
{
	udev->dev = input_allocate_device();
	if (!udev->dev)
		return -ENOMEM;

	udev->dev->event = uinput_dev_event;
	input_set_drvdata(udev->dev, udev);

	return 0;
}

static int uinput_setup_device(struct uinput_device *udev, const char __user *buffer, size_t count)
{
	struct uinput_user_dev	*user_dev;
	struct input_dev	*dev;
	int			i;
	int			retval;

	if (count != sizeof(struct uinput_user_dev))
		return -EINVAL;

	if (!udev->dev) {
		retval = uinput_allocate_device(udev);
		if (retval)
			return retval;
	}

	dev = udev->dev;
	#ifdef MOUSE_INPUT_THREAD
	udev_wow = udev->dev;
	#endif

	user_dev = memdup_user(buffer, sizeof(struct uinput_user_dev));
	if (IS_ERR(user_dev))
		return PTR_ERR(user_dev);

	udev->ff_effects_max = user_dev->ff_effects_max;

	/* Ensure name is filled in */
	if (!user_dev->name[0]) {
		retval = -EINVAL;
		goto exit;
	}

	kfree(dev->name);
	dev->name = kstrndup(user_dev->name, UINPUT_MAX_NAME_SIZE,
			     GFP_KERNEL);
	if (!dev->name) {
		retval = -ENOMEM;
		goto exit;
	}

	dev->id.bustype	= user_dev->id.bustype;
	dev->id.vendor	= user_dev->id.vendor;
	dev->id.product	= user_dev->id.product;
	dev->id.version	= user_dev->id.version;

#if 1
	for (i = 0; i < ABS_CNT; i++) {
		input_abs_set_max(dev, i, user_dev->absmax[i]);
		input_abs_set_min(dev, i, user_dev->absmin[i]);
		input_abs_set_fuzz(dev, i, user_dev->absfuzz[i]);
		input_abs_set_flat(dev, i, user_dev->absflat[i]);
	}

	/* check if absmin/absmax/absfuzz/absflat are filled as
	 * told in Documentation/input/input-programming.txt */
	if (test_bit(EV_ABS, dev->evbit)) {
		retval = uinput_validate_absbits(dev);
		if (retval < 0)
			goto exit;
		if (test_bit(ABS_MT_SLOT, dev->absbit)) {
			int nslot = input_abs_get_max(dev, ABS_MT_SLOT) + 1;
			input_mt_init_slots(dev, nslot);
		} else if (test_bit(ABS_MT_POSITION_X, dev->absbit)) {
			input_set_events_per_packet(dev, 60);
		}
	}
#endif

	udev->state = UIST_SETUP_COMPLETE;
	retval = count;

 exit:
	kfree(user_dev);
	return retval;
}

#ifdef MOUSE_INPUT_THREAD

static int max_count = 0;

static int remocon_thread(void *data)
{
	for(;;)
	{
		int cnt = 0;
	
		if(rd_data_cnt != wr_data_cnt)
		{
			if(rd_data_cnt > wr_data_cnt)
				cnt = MAX_CNT_UINPUT - rd_data_cnt + wr_data_cnt;
			else
				cnt = wr_data_cnt - rd_data_cnt;

			if( cnt > max_count)
			{
				max_count = cnt;
				printk("WOW : %d\n", max_count);
			}
		
			input_event(udev_wow, rd_mouse_cnt[rd_data_cnt][0], rd_mouse_cnt[rd_data_cnt][1], rd_mouse_cnt[rd_data_cnt][2]);

			rd_data_cnt++;
			if(rd_data_cnt >= MAX_CNT_UINPUT)
				rd_data_cnt = 0;
		}
		else
		{
			msleep(1);
		}
	}
}
#endif

static inline ssize_t uinput_inject_event(struct uinput_device *udev, const char __user *buffer, size_t count)
{
	struct input_event ev;

	if (count < input_event_size())
		return -EINVAL;

	if (input_event_from_user(buffer, &ev))
		return -EFAULT;
	
#ifdef MOUSE_INPUT_THREAD

	rd_mouse_cnt[wr_data_cnt][0] = ev.type;
	rd_mouse_cnt[wr_data_cnt][1] = ev.code;
	rd_mouse_cnt[wr_data_cnt][2] = ev.value;
	
	wr_data_cnt++;
	if(wr_data_cnt >= MAX_CNT_UINPUT)
		wr_data_cnt = 0;

	return 1;
#else
	input_event(udev->dev, ev.type, ev.code, ev.value);
	return input_event_size();

#endif

}

static ssize_t tcc_uinput_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	struct uinput_device *udev = file->private_data;
	int retval;
	struct input_event ev;
	int sec_gap = 0;
	
	//if (input_event_from_user(buffer, &ev))
	//		return -EFAULT;

	do_gettimeofday(&oldtv);
#if 0//def GMAC_TIME_COMPARE
					if(ev.code ==0x00 && ev.type== EV_REL){
					
					do_gettimeofday(&newtv);
						int sec_gap = newtv.tv_sec - oldtv.tv_sec;
						
						gap_time[time_cnt] =((sec_gap*1000000+newtv.tv_usec )- (oldtv.tv_usec))/1000;
						total_time += gap_time[time_cnt];
						//if(time_cnt != 0 && time_cnt % (MAX_GAP_CNT-1) == 0){
					
							
							total_times[total_time_cnt] = total_time;
							total_sum_time += total_times[total_time_cnt];
							if(total_time_cnt != 0 && total_time_cnt % (MAX_CNT-1) == 0)
							{
							
							printk("Uinput: avg:%d ms: %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d \n",
								total_sum_time/MAX_CNT, total_times[0], total_times[1], total_times[2], total_times[3], total_times[4], total_times[5], total_times[6], total_times[7], total_times[8], total_times[9], 
								total_times[10], total_times[11], total_times[12], total_times[13], total_times[14], total_times[15], total_times[16], total_times[17], total_times[18], total_times[19], 
								total_times[20], total_times[21], total_times[22], total_times[23], total_times[24], total_times[25], total_times[26], total_times[27], total_times[28], total_times[29]);
					
								total_time_cnt=0;
								total_byte =0;
								total_sum_time =0;
							}
							else{
								total_time_cnt++;
							}
							time_cnt = 0;
							//total_byte =0;
							total_time =0;
					
						//}
						//else{
						//	time_cnt++;
						//}
						
					oldtv=newtv;
						}
#endif

	//printk("1\n");

	retval = mutex_lock_interruptible(&udev->mutex);
	if (retval){
		printk("retval %d\n",retval);
		return retval;
	}

		retval = udev->state == UIST_CREATED ?
			uinput_inject_event(udev, buffer, count) :
			uinput_setup_device(udev, buffer, count);

	//if(ev.code == 0)
	//printk("input %d\n",ev.value);
	if(ev.code ==0x00 && ev.type== EV_REL){
		//do_gettimeofday(&first);
		//printk("1 %d \n",((sec_gap*1000000+first.tv_usec )- (oldtv.tv_usec))/1000);
	}
	mutex_unlock(&udev->mutex);
	//printk("2\n");
	
	//if(ev.code ==0x00 && ev.type== EV_REL){
		do_gettimeofday(&newtv);
		sec_gap = newtv.tv_sec - oldtv.tv_sec;
		//printk("2 %d \n",((sec_gap*1000000+newtv.tv_usec )- (oldtv.tv_usec))/1000);
	//}

	return retval;
}

static ssize_t tcc_uinput_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct uinput_device *udev = file->private_data;
	int retval = 0;

	if (udev->state != UIST_CREATED)
		return -ENODEV;

	if (udev->head == udev->tail && (file->f_flags & O_NONBLOCK))
		return -EAGAIN;

	retval = wait_event_interruptible(udev->waitq,
			udev->head != udev->tail || udev->state != UIST_CREATED);
	if (retval)
		return retval;

	retval = mutex_lock_interruptible(&udev->mutex);
	if (retval)
		return retval;

	if (udev->state != UIST_CREATED) {
		retval = -ENODEV;
		goto out;
	}

	while (udev->head != udev->tail && retval + input_event_size() <= count) {
		if (input_event_to_user(buffer + retval, &udev->buff[udev->tail])) {
			retval = -EFAULT;
			goto out;
		}
		udev->tail = (udev->tail + 1) % UINPUT_BUFFER_SIZE;
		retval += input_event_size();
	}

 out:
	mutex_unlock(&udev->mutex);

	return retval;
}

static unsigned int tcc_uinput_poll(struct file *file, poll_table *wait)
{
	struct uinput_device *udev = file->private_data;

	poll_wait(file, &udev->waitq, wait);

	if (udev->head != udev->tail)
		return POLLIN | POLLRDNORM;

	return 0;
}

static int tcc_uinput_release(struct inode *inode, struct file *file)
{
	struct uinput_device *udev = file->private_data;

	uinput_destroy_device(udev);
	kfree(udev);

	return 0;
}

#ifdef CONFIG_COMPAT
struct uinput_ff_upload_compat {
	int			request_id;
	int			retval;
	struct ff_effect_compat	effect;
	struct ff_effect_compat	old;
};

static int uinput_ff_upload_to_user(char __user *buffer,
				    const struct uinput_ff_upload *ff_up)
{
	if (INPUT_COMPAT_TEST) {
		struct uinput_ff_upload_compat ff_up_compat;

		ff_up_compat.request_id = ff_up->request_id;
		ff_up_compat.retval = ff_up->retval;
		/*
		 * It so happens that the pointer that gives us the trouble
		 * is the last field in the structure. Since we don't support
		 * custom waveforms in uinput anyway we can just copy the whole
		 * thing (to the compat size) and ignore the pointer.
		 */
		memcpy(&ff_up_compat.effect, &ff_up->effect,
			sizeof(struct ff_effect_compat));
		memcpy(&ff_up_compat.old, &ff_up->old,
			sizeof(struct ff_effect_compat));

		if (copy_to_user(buffer, &ff_up_compat,
				 sizeof(struct uinput_ff_upload_compat)))
			return -EFAULT;
	} else {
		if (copy_to_user(buffer, ff_up,
				 sizeof(struct uinput_ff_upload)))
			return -EFAULT;
	}

	return 0;
}

static int uinput_ff_upload_from_user(const char __user *buffer,
				      struct uinput_ff_upload *ff_up)
{
	if (INPUT_COMPAT_TEST) {
		struct uinput_ff_upload_compat ff_up_compat;

		if (copy_from_user(&ff_up_compat, buffer,
				   sizeof(struct uinput_ff_upload_compat)))
			return -EFAULT;

		ff_up->request_id = ff_up_compat.request_id;
		ff_up->retval = ff_up_compat.retval;
		memcpy(&ff_up->effect, &ff_up_compat.effect,
			sizeof(struct ff_effect_compat));
		memcpy(&ff_up->old, &ff_up_compat.old,
			sizeof(struct ff_effect_compat));

	} else {
		if (copy_from_user(ff_up, buffer,
				   sizeof(struct uinput_ff_upload)))
			return -EFAULT;
	}

	return 0;
}

#else

static int uinput_ff_upload_to_user(char __user *buffer,
				    const struct uinput_ff_upload *ff_up)
{
	if (copy_to_user(buffer, ff_up, sizeof(struct uinput_ff_upload)))
		return -EFAULT;

	return 0;
}

static int uinput_ff_upload_from_user(const char __user *buffer,
				      struct uinput_ff_upload *ff_up)
{
	if (copy_from_user(ff_up, buffer, sizeof(struct uinput_ff_upload)))
		return -EFAULT;

	return 0;
}

#endif

#define uinput_set_bit(_arg, _bit, _max)		\
({							\
	int __ret = 0;					\
	if (udev->state == UIST_CREATED)		\
		__ret =  -EINVAL;			\
	else if ((_arg) > (_max))			\
		__ret = -EINVAL;			\
	else set_bit((_arg), udev->dev->_bit);		\
	__ret;						\
})

static long tcc_uinput_ioctl_handler(struct file *file, unsigned int cmd,
				 unsigned long arg, void __user *p)
{

	int			retval;
	struct uinput_device	*udev = file->private_data;
	struct uinput_ff_upload ff_up;
	struct uinput_ff_erase  ff_erase;
	struct uinput_request   *req;
	char			*phys;
	retval = mutex_lock_interruptible(&udev->mutex);
	if (retval){
		return retval;
	}

	if (!udev->dev) {
		retval = uinput_allocate_device(udev);
		if (retval){
			goto out;
		}
	}

	switch (cmd) {
		case UI_DEV_CREATE:
			retval = uinput_create_device(udev);
			break;

		case UI_DEV_DESTROY:
			uinput_destroy_device(udev);
			break;

		case UI_SET_EVBIT:
			retval = uinput_set_bit(arg, evbit, EV_MAX);
			break;

		case UI_SET_KEYBIT:
			retval = uinput_set_bit(arg, keybit, KEY_MAX);
			break;

		case UI_SET_RELBIT:
			retval = uinput_set_bit(arg, relbit, REL_MAX);
			break;

		case UI_SET_ABSBIT:
			retval = uinput_set_bit(arg, absbit, ABS_MAX);
			break;

		case UI_SET_MSCBIT:
			retval = uinput_set_bit(arg, mscbit, MSC_MAX);
			break;

		case UI_SET_LEDBIT:
			retval = uinput_set_bit(arg, ledbit, LED_MAX);
			break;

		case UI_SET_SNDBIT:
			retval = uinput_set_bit(arg, sndbit, SND_MAX);
			break;

		case UI_SET_FFBIT:
			retval = uinput_set_bit(arg, ffbit, FF_MAX);
			break;

		case UI_SET_SWBIT:
			retval = uinput_set_bit(arg, swbit, SW_MAX);
			break;

		case UI_SET_PROPBIT:
			retval = uinput_set_bit(arg, propbit, INPUT_PROP_MAX);
			break;

		case UI_SET_PHYS:
			if (udev->state == UIST_CREATED) {
				retval = -EINVAL;
				goto out;
			}

			phys = strndup_user(p, 1024);
			if (IS_ERR(phys)) {
				retval = PTR_ERR(phys);
				goto out;
			}

			kfree(udev->dev->phys);
			udev->dev->phys = phys;
			break;

		case UI_BEGIN_FF_UPLOAD:
			retval = uinput_ff_upload_from_user(p, &ff_up);
			if (retval)
				break;

			req = uinput_request_find(udev, ff_up.request_id);
			if (!req || req->code != UI_FF_UPLOAD || !req->u.upload.effect) {
				retval = -EINVAL;
				break;
			}

			ff_up.retval = 0;
			ff_up.effect = *req->u.upload.effect;
			if (req->u.upload.old)
				ff_up.old = *req->u.upload.old;
			else
				memset(&ff_up.old, 0, sizeof(struct ff_effect));

			retval = uinput_ff_upload_to_user(p, &ff_up);
			break;

		case UI_BEGIN_FF_ERASE:
			if (copy_from_user(&ff_erase, p, sizeof(ff_erase))) {
				retval = -EFAULT;
				break;
			}

			req = uinput_request_find(udev, ff_erase.request_id);
			if (!req || req->code != UI_FF_ERASE) {
				retval = -EINVAL;
				break;
			}

			ff_erase.retval = 0;
			ff_erase.effect_id = req->u.effect_id;
			if (copy_to_user(p, &ff_erase, sizeof(ff_erase))) {
				retval = -EFAULT;
				break;
			}

			break;

		case UI_END_FF_UPLOAD:
			retval = uinput_ff_upload_from_user(p, &ff_up);
			if (retval)
				break;

			req = uinput_request_find(udev, ff_up.request_id);
			if (!req || req->code != UI_FF_UPLOAD ||
			    !req->u.upload.effect) {
				retval = -EINVAL;
				break;
			}

			req->retval = ff_up.retval;
			uinput_request_done(udev, req);
			break;

		case UI_END_FF_ERASE:
			if (copy_from_user(&ff_erase, p, sizeof(ff_erase))) {
				retval = -EFAULT;
				break;
			}

			req = uinput_request_find(udev, ff_erase.request_id);
			if (!req || req->code != UI_FF_ERASE) {
				retval = -EINVAL;
				break;
			}

			req->retval = ff_erase.retval;
			uinput_request_done(udev, req);
			break;

		default:
			retval = -EINVAL;
	}

 out:
	mutex_unlock(&udev->mutex);
	return retval;
	
}

static long tcc_uinput_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return tcc_uinput_ioctl_handler(file, cmd, arg, (void __user *)arg);
}

#ifdef CONFIG_COMPAT
static long tcc_uinput_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return tcc_uinput_ioctl_handler(file, cmd, arg, compat_ptr(arg));
}
#endif

static const struct file_operations uinput_fops = {
	.owner		= THIS_MODULE,
	.open		= tcc_uinput_open,
	.release	= tcc_uinput_release,
	.read		= tcc_uinput_read,
	.write		= tcc_uinput_write,
	.poll		= tcc_uinput_poll,
	.unlocked_ioctl	= tcc_uinput_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= tcc_uinput_compat_ioctl,
#endif
	.llseek		= no_llseek,
};

static struct miscdevice tcc_uinput_misc = {
	.fops		= &uinput_fops,
	.minor		= TCC_UINPUT_MINOR,
	.name		= TCC_UINPUT_NAME,
};
MODULE_ALIAS_MISCDEV(TCC_UINPUT_MINOR);
MODULE_ALIAS("devname:" TCC_UINPUT_NAME);

static int __init tcc_uinput_init(void)
{
	return misc_register(&tcc_uinput_misc);
}

static void __exit tcc_uinput_exit(void)
{
	misc_deregister(&tcc_uinput_misc);
}

MODULE_AUTHOR("Aristeu Sergio Rozanski Filho");
MODULE_DESCRIPTION("User level driver support for input subsystem");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.3");

module_init(tcc_uinput_init);
module_exit(tcc_uinput_exit);

