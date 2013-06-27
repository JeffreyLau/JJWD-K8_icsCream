#pragma version(1)
#pragma rs java_package_name(android.renderscript.cts)

void root(const float2 *ain, float2 *aout) {
    aout[0].x = ain[0].x + 1.0f;
    aout[0].y = ain[0].y + 1.0f;
    return;
}
