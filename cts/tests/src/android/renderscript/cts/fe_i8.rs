#pragma version(1)
#pragma rs java_package_name(android.renderscript.cts)

void root(const char *ain, uchar *aout) {
    aout[0] = ain[0] + 1;
    return;
}
