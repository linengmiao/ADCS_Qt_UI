#ifndef TRIAD_H
#define TRIAD_H

#include "widget.h"

class TRIAD
{
public:
    TRIAD();
    ~TRIAD();

        void setupVectors(float *Mi, float *Mo, float *Si, float *So,float *RM);
        void calcTRIAD(float *Mi, float *Mo, float *Si, float *So, float *RM);


        float getAngleRM(int angleNr);
        void setAngleRM(int angleNr, float value);




        void regroupMatrix2(float *pi, float *pi2, float *pi3, float *outputMatrix);
        void calcAnglesRM(float finalRotationMatrix[3][3]);
        void createFirstRotMatrix(float *firstRotMatrix);
        void workaroundMallocFail3x3(float *array0, float *array1, float output[3][3]);
        void transposeMatrix(float *array2, int height, int width, float *output, int height2, int width2);
        void multiplyMatrices(float *transposedMatrix, int height1, int width1, float *iSunVector, int height2, int width2, float *orbitalSunVector);
        void multiplyCrossVectors2(float *first, float *second, float *output);
        void normalizeVector2(float *input, float *normalizedVec);
        void multiplyMatrices3x1(float *array0, float *array1, float *arrayOut);
        void calculateTMFV(float* orbitalTMFV, int futurePosition);
private:
            float epsilonClasse;
        float theta;
        float random;
        float phi;



};

#endif // TRIAD_H
