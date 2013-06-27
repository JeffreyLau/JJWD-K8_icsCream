/*
 * drivers/net/phy/telechips_lan8720.c
 *
 * Driver for Telechips SMSC PHYs
 *
 * Author: Telechips
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/netdevice.h>

#define MII_LAN83C185_ISF 29 /* Interrupt Source Flags */
#define MII_LAN83C185_IM  30 /* Interrupt Mask */
#define MII_LAN83C185_CTRL_STATUS 17 /* Mode/Status Register */

#define MII_LAN83C185_ISF_INT1 (1<<1) /* Auto-Negotiation Page Received */
#define MII_LAN83C185_ISF_INT2 (1<<2) /* Parallel Detection Fault */
#define MII_LAN83C185_ISF_INT3 (1<<3) /* Auto-Negotiation LP Ack */
#define MII_LAN83C185_ISF_INT4 (1<<4) /* Link Down */
#define MII_LAN83C185_ISF_INT5 (1<<5) /* Remote Fault Detected */
#define MII_LAN83C185_ISF_INT6 (1<<6) /* Auto-Negotiation complete */
#define MII_LAN83C185_ISF_INT7 (1<<7) /* ENERGYON */

#define MII_LAN83C185_ISF_INT_ALL (0x0e)

#define MII_LAN83C185_ISF_INT_PHYLIB_EVENTS \
	(MII_LAN83C185_ISF_INT6 | MII_LAN83C185_ISF_INT4 | \
	 MII_LAN83C185_ISF_INT7)

#define MII_LAN83C185_EDPWRDOWN	(1 << 13) /* EDPWRDOWN */

static int smsc_phy_config_intr(struct phy_device *phydev)
{
	int rc = phy_write (phydev, MII_LAN83C185_IM,
			((PHY_INTERRUPT_ENABLED == phydev->interrupts)
			? MII_LAN83C185_ISF_INT_PHYLIB_EVENTS
			: 0));

	return rc < 0 ? rc : 0;
}

static int smsc_phy_ack_interrupt(struct phy_device *phydev)
{
	int rc = phy_read (phydev, MII_LAN83C185_ISF);

	return rc < 0 ? rc : 0;
}

static int smsc_phy_config_init(struct phy_device *phydev)
{
	return smsc_phy_ack_interrupt (phydev);
}

static struct phy_driver lan8720_driver = {
	.phy_id		= 0x0007c0f0, /* OUI=0x00800f, Model#=0x0f */
	.phy_id_mask	= 0xfffffff0,
	.name		= "Telechips SMSC LAN8720",

	.features	= (PHY_BASIC_FEATURES | SUPPORTED_Pause
				| SUPPORTED_Asym_Pause),
	//.flags		= PHY_HAS_INTERRUPT | PHY_HAS_MAGICANEG,
	//.flags		= PHY_POLL | PHY_HAS_MAGICANEG,
	.flags		= PHY_POLL,

	/* basic functions */
	.config_aneg	= genphy_config_aneg,
	.read_status	= genphy_read_status,
	.config_init	= smsc_phy_config_init,

	/* IRQ related */
	.ack_interrupt	= smsc_phy_ack_interrupt,
	.config_intr	= smsc_phy_config_intr,

	.suspend	= genphy_suspend,
	.resume		= genphy_resume,

	.driver		= { .owner = THIS_MODULE, }
};

static int __init smsc_init(void)
{
	int ret;


	ret = phy_driver_register (&lan8720_driver);
	if (ret)
		return ret;

	return 0;
}

static void __exit smsc_exit(void)
{
	phy_driver_unregister (&lan8720_driver);
}

MODULE_DESCRIPTION("Telechips SMSC PHY driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");

module_init(smsc_init);
module_exit(smsc_exit);

static struct mdio_device_id __maybe_unused smsc_tbl[] = {
	{ 0x0007c0f0, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, smsc_tbl);
