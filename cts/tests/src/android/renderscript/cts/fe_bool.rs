#pragma version(1)
#pragma rs java_package_name(android.renderscript.cts)

void root(const bool *ain, bool *aout) {
    aout[0] = !ain[0];
    return;
}
