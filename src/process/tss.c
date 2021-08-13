#include "tss.h"

void tss_init(){
    tss=palloc(1,F_ASSERT| F_ZERO);
}