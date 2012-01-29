#ifndef _PAYSAGES_MAIN_H_
#define _PAYSAGES_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

void paysagesInit();
void paysagesSave(char* filepath);
void paysagesLoad(char* filepath);

#ifdef __cplusplus
}
#endif

#endif
