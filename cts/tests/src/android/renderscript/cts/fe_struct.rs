#pragma version(1)
#pragma rs java_package_name(android.renderscript.cts)

struct fe_test {
    int i;
    float f;
};

void root(const struct fe_test *ain, struct fe_test *aout) {
    aout[0].i = ain[0].i + 1;
    aout[0].f = ain[0].f + 1.0f;
    return;
}
