#pragma version(1)
#pragma rs java_package_name(android.renderscript.cts)

void root(const float *ain, float *aout) {
    aout[0] = ain[0] + 1.0f;
    return;
}
