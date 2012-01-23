#ifndef _PAYSAGES_FUNCTIONS_H_
#define _PAYSAGES_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include <stdio.h>

void paysagesInit();
void paysagesSave(char* filepath);
void paysagesLoad(char* filepath);

/* array.c */
void arrayCreate(Array* array, int item_size);
void arrayDelete(Array* array);
void* arrayAppend(Array* array, void* item);
void arrayInsert(Array* array, void* item, int position);
void arrayReplace(Array* array, void* item, int position);
void arrayLStrip(Array* array, int count);
void arrayClear(Array* array);

/* auto.c */
void autoInit();
void autoSetDaytime(int hour, int minute);
void autoSetDaytimeFraction(double daytime);
void autoGenRealisticLandscape(int seed);
void autoRenderSceneTwoPass(int postonly);
void autoRenderSceneRayTracing();

/* color.c */
void colorSave(Color col, FILE* f);
Color colorLoad(FILE* f);
unsigned int colorTo32BitRGBA(Color* col);
unsigned int colorTo32BitBGRA(Color* col);
unsigned int colorTo32BitARGB(Color* col);
unsigned int colorTo32BitABGR(Color* col);
void colorMask(Color* base, Color* mask);
double colorNormalize(Color* col);
double colorGetValue(Color* col);
ColorGradation colorGradationCreate();
void colorGradationAdd(ColorGradation* gradation, double value, Color* col);
void colorGradationAddRgba(ColorGradation* gradation, double value, double r, double g, double b, double a);
Color colorGradationGet(ColorGradation* gradation, double value);
void colorGradationSave(FILE* f, ColorGradation gradation);
ColorGradation colorGradationLoad(FILE* f);

/* euclid.c */
void v3Save(Vector3 v, FILE* f);
Vector3 v3Load(FILE* f);
void m4Save(Matrix4 m, FILE* f);
Matrix4 m4Load(FILE* f);
Vector3 v3Translate(Vector3 v1, double x, double y, double z);
Vector3 v3Add(Vector3 v1, Vector3 v2);
Vector3 v3Sub(Vector3 v1, Vector3 v2);
Vector3 v3Neg(Vector3 v);
Vector3 v3Scale(Vector3 v, double scale);
double v3Norm(Vector3 v);
Vector3 v3Normalize(Vector3 v);
double v3Dot(Vector3 v1, Vector3 v2);
Vector3 v3Cross(Vector3 v1, Vector3 v2);
Matrix4 m4NewIdentity();
Matrix4 m4Mult(Matrix4 m1, Matrix4 m2);
Vector3 m4MultPoint(Matrix4 m, Vector3 v);
Vector3 m4Transform(Matrix4 m, Vector3 v);
Matrix4 m4Transpose(Matrix4 m);
Matrix4 m4NewScale(double x, double y, double z);
Matrix4 m4NewTranslate(double x, double y, double z);
Matrix4 m4NewRotateX(double angle);
Matrix4 m4NewRotateY(double angle);
Matrix4 m4NewRotateZ(double angle);
Matrix4 m4NewRotateAxis(double angle, Vector3 axis);
Matrix4 m4NewRotateEuler(double heading, double attitude, double bank);
Matrix4 m4NewRotateTripleAxis(Vector3 x, Vector3 y, Vector3 z);
Matrix4 m4NewLookAt(Vector3 eye, Vector3 at, Vector3 up);
Matrix4 m4NewPerspective(double fov_y, double aspect, double near, double far);
double m4Determinant(Matrix4 m);
Matrix4 m4Inverse(Matrix4 m);

/* fog.c */
void fogSave(FILE* f);
void fogLoad(FILE* f);
void fogSetColor(Color col);
void fogSetDistance(double near, double far);
Color fogApplyToLocation(Vector3 location, Color base);

/* render.c */
void renderInit();
void renderSave(FILE* f);
void renderLoad(FILE* f);
void renderSetSize(int width, int height);
int renderSetQuality(int quality);
void renderClear();
void renderUpdate();
void renderInterrupt();
void renderSetBackgroundColor(Color* col);
double renderGetPrecision(Vector3 location);
void renderAddFragment(RenderFragment* fragment);
void renderPushFragment(int x, int y, double z, Vertex* vertex);
void renderPushTriangle(Vertex* v1, Vertex* v2, Vertex* v3);
void renderPushQuad(Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);
void renderPostProcess(int nbchunks);
void renderSaveToFile(const char* path);
void renderSetPreviewCallbacks(PreviewCallbackResize resize, PreviewCallbackClear clear, PreviewCallbackDraw draw, PreviewCallbackUpdate update);
int renderSetNextProgressStep(double start, double end);
int renderTellProgress(double progress);

/* tools.c */
double toolsRandom();
double toolsBicubicInterpolate(double stencil[16], double x, double y);
void toolsFloat2DMapCopy(double* src, double* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep);
Vector3 toolsGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
void toolsSaveDouble(FILE* f, double value);
double toolsLoadDouble(FILE* f);
void toolsSaveInt(FILE* f, int value);
int toolsLoadInt(FILE* f);

/* zone.c */
Zone* zoneCreate();
void zoneDelete(Zone* zone);
void zoneSave(Zone* zone, FILE* f);
void zoneLoad(Zone* zone, FILE* f);
void zoneCopy(Zone* source, Zone* destination);
void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius);
void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius);
void zoneAddHeightRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);
void zoneAddSteepnessRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);
double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#ifdef __cplusplus
}
#endif

#endif
