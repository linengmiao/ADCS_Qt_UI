/****************************************************************************
 * This class is used to check the TRIAD algorithm. The actual
 * calculations are made on the MCU itself. Both, the Qt UI and the MCU,
 * are based on the same code, to provide the checking-part a maximum
 * fiability
 *
 *
 ****************************************************************************/


#include "triad.h"
#include "ui_widget.h"
#include "widget.h"

#define PI 3.14159263839

TRIAD::TRIAD()
{
  setAngleRM(1,20);
  setAngleRM(2,40);
  setAngleRM(3,30);
}

TRIAD::~TRIAD()
{

}

/*****************************************************************************/
//applies the TRIAD algorithm with the input values passed as parameters
// returns a 1D array made out of 9 values representing the final RM

void TRIAD::calcTRIAD(float *Mi, float *Si, float *Mo, float *So, float *RM)
{
    int i,j;

    float to2[3] = {0};
    float ti2[3] = {0};

    float Pib[3] = {0};
    float Pob[3] = {0};
    float Pi2b[3] = {0};
    float Po2b[3] = {0};

    float Pi3b[3]={0};
    float Po3b[3]= {0};

    float PiMatrixb[9]={0};
    float PoMatrixb[9]={0};
    float PoMatrixtb[9] = {0};

    float finalRotationMatrix[3][3]={0};

   /* float Mo2[3],Mi2[3],So2[3],Si2[3];

    for(i=0;i<3;i++)
    {
        Mo2[i] = Mo[i];
        Mi2[i] = Mi[i];
        So2[i] = So[i];
        Si2[i] = Si[i];
    }*/

        multiplyCrossVectors2(Mo, So, to2);
        multiplyCrossVectors2(Mi, Si, ti2);
        normalizeVector2(Mo, Pob);
        normalizeVector2(Mi, Pib);
        normalizeVector2(ti2, Pi2b);
        multiplyCrossVectors2(Pib, Pi2b, Pi3b);
        regroupMatrix2(Pib,Pi2b, Pi3b, PiMatrixb);


        qDebug("Mo");
        for(i=0;i<3;i++)
        {qDebug("%f",Mo[i]);}
        qDebug("So");
        for(i=0;i<3;i++)
        {qDebug("%f",So[i]);}

        qDebug("to2");
        for(i=0;i<3;i++)
        {qDebug("%f",to2[i]);}

        qDebug("Mi");
        for(i=0;i<3;i++)
        {qDebug("%f",Mi[i]);}
        qDebug("Si");
        for(i=0;i<3;i++)
        {qDebug("%f",Si[i]);}


        qDebug("PiMatrixb");
        for(i=0;i<9;i++)
        {qDebug("%f",PiMatrixb[i]);}

        normalizeVector2(to2, Po2b);

        qDebug("po2b");
        for(i=0;i<3;i++)
        {qDebug("%f",Po2b[i]);}

        multiplyCrossVectors2(Pob, Po2b, Po3b);
        qDebug("po3b");
        for(i=0;i<3;i++)
        {qDebug("%f",Po3b[i]);}
        regroupMatrix2(Pob,Po2b, Po3b, PoMatrixb);

        qDebug("PoMatrixb");
        for(i=0;i<9;i++)
        {qDebug("%f",PoMatrixb[i]);}

        transposeMatrix(PoMatrixb, 3 ,3, PoMatrixtb,3 ,3);
        workaroundMallocFail3x3(PiMatrixb, PoMatrixtb, finalRotationMatrix);


        RM[0] = (finalRotationMatrix[0][0]);  //zie p 4-16 exacte Rtriad
        RM[1] = (finalRotationMatrix[0][1]);
        RM[2] = (finalRotationMatrix[0][2]);
        RM[3] = (finalRotationMatrix[1][0]);
        RM[4] = (finalRotationMatrix[1][1]);
        RM[5] = (finalRotationMatrix[1][2]);
        RM[6] = (finalRotationMatrix[2][0]);
        RM[7] = (finalRotationMatrix[2][1]);
        RM[8] = (finalRotationMatrix[2][2]);

        qDebug("\nfinal RM 9:");
        for(i=0;i<9;i++)
        {
          qDebug()<<RM[i];
        }

      //  calcAnglesRM(finalRotationMatrix);

        qDebug("---------------------------");

}

float TRIAD::getAngleRM(int angleNr)
{
    if(angleNr==1)
    {return epsilonClasse;}
    else if(angleNr==2)
    {return theta;}
    else if(angleNr==3)
    {return phi;}
}
/*****************************************************************************/
// calculates the other pair of vectors in the other ref. frame by calculations
// with the RM from the values of the sliders

void TRIAD::setupVectors(float *Mi,float *Mo,float *Si,float *So,float *RM)
{
    int i=0;
    static int iter=0;

    float setupRM[9]={0};
    float setupRMt[9]={0};
/*
    Si[0] = 36650;
    Si[1] = 1;
    Si[2] = 2;

    Mo[0] = 5 ;
    Mo[1] = 9 ;
    Mo[2] = 10 ;
*/
    if(iter==0)
    {
        createFirstRotMatrix(setupRM);
        for(i=0;i<9;i++)
        {
            RM[i] = setupRM[i];
        }

    }
    iter++;
    qDebug("\setup RM 9:");
    for(i=0;i<9;i++)
    {
      qDebug()<<RM[i];
    }
    transposeMatrix(RM, 3, 3, setupRMt, 3, 3);
    multiplyMatrices3x1(setupRMt, Si, So);
    calculateTMFV(Mo,1);
    multiplyMatrices3x1(RM, Mo, Mi);

/*    qDebug("setup Mi");
    for(i=0;i<3;i++)
    {
        qDebug()<<Mi[i];
    }

    qDebug("setup Mo");
    for(i=0;i<3;i++)
    {
        qDebug()<<Mo[i];
    }

    qDebug("setup So");
    for(i=0;i<3;i++)
    {
        qDebug()<<So[i];
    }


    qDebug("setup Si");
    for(i=0;i<3;i++)
    {
        qDebug()<<Si[i];
    }
*/

if (iter==3)
{iter=0;}
}



void TRIAD::calculateTMFV(float* orbitalTMFV, int futurePosition)
{
/*runge kutta loop*/
    float x0=0, y0=0, z0=0;
    float Vx0=0, Vy0=0, Vz0=0;
    int i=0;
    float h=10; //stapgrootte
    float px=0, py=0,pz=0, pvX=0, pvY=0, pvZ=0;
    //float fsdfsd = (3.0f/2.0f) * pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * x0 * (1- ((5*pow(z0,2))/pow(r,2))) ;
    float qx=0, qy, qz=0,qvx=0, qvy=0, qvz=0;
    float rx=0, ry=0, rz=0, rvx=0, rvy=0, rvz=0;
    float sx=0, sy=0, sz=0, svx=0, svy=0, svz=0;
    float x1=0, y1=0, z1=0, vx1=0, vy1=0, vz1=0;
    //float sdasfd= (1.0f/6.0f) * h * (px + 2*qx + 2*rx + sx);

/*final results*/
    float c=0,c1=0, c2=0, c3=0;
    float l=0,u=0;
    float iTMF=0;


 /*motion model*/
        float micro=398600;
        float n0=  14.70988142103530; //Mean Motion
        float e = 0.001017;
        //double M_PI = 3.14;
        float meanAnomaly = 201.94908*PI/180.0;
        float argumentPerigee = 158.2168*PI/180.0; //u
        float rightAscentionAscNode = 185.6639*PI/180.0; //omega
        float inclination = 98.0719*PI/180.0;


        /*float meanAnomaly = iMeanAnomaly*PI/180.0;
        float argumentPerigee = iArgumentPerigee*PI/180.0;
        float rightAscentionAscNode = irightAscentionAscNode*PI/180.0; //omega
        float inclination = iinclination*PI/180.0;
*/
        /*prediction*/
        float j = -1082e-6;
        float ae = 6378.136;

        float Tobr = 86400.0/n0;
        float ke=sqrt(micro);
        /*return*/
  //      float tmfv[6]={0};
                /*starting values for first iteration*/

                float a=0, r=0, Vr=0, Vn=0;

                a = pow(ke*Tobr/2.0/PI,0.666);
                //a  = (sqrt(micro)/n0);

                r = a*(1- e* cos(micro)); //return value in radians
                Vr = sqrt(micro/a) * ((e * sin(meanAnomaly))/sqrt(1-(pow(e,2))));
                Vn = sqrt(micro/a) * ((e * 1+cos(meanAnomaly))/sqrt(1-(pow(e,2))));


                x0 = r*((cos(argumentPerigee) * cos(rightAscentionAscNode)) - (sin(argumentPerigee) * sin(rightAscentionAscNode) * cos(inclination)));
                y0 = r*((cos(argumentPerigee) * sin(rightAscentionAscNode)) + (sin(argumentPerigee) * cos(rightAscentionAscNode) * cos(inclination)));
                z0 = r * sin(argumentPerigee) * sin(inclination);


                Vx0 = Vr *(cos(rightAscentionAscNode)*cos(argumentPerigee) - sin(rightAscentionAscNode)*sin(argumentPerigee)*cos(inclination)) - Vn*(cos(rightAscentionAscNode)*sin(argumentPerigee) + sin(rightAscentionAscNode)*cos(argumentPerigee)*cos(inclination));
                Vy0 = Vr *(sin(rightAscentionAscNode)*cos(argumentPerigee) - cos(rightAscentionAscNode)*sin(argumentPerigee)*cos(inclination)) + Vn*(sin(rightAscentionAscNode)*sin(argumentPerigee) + cos(rightAscentionAscNode)*cos(argumentPerigee)*cos(inclination));
                Vz0 = Vr * sin(argumentPerigee)*sin(inclination) + Vr * cos(argumentPerigee) * sin(inclination);



                for (i=0;i<futurePosition;i++)
                {

            /*Prediction*/

                r=0; //previously defined variable

                r = pow(x0,2)+pow(y0,2)+pow(z0,2);
                r = sqrt(r);

                px = Vx0;
                py = Vy0;
                pz = Vz0;


                pvX = (-(micro/(pow(r,3))) * x0) - (3.0f/2.0f) * pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * x0 * (1- ((5*pow(z0,2))/pow(r,2))) ;
                pvY = (-(micro/(pow(r,3))) * y0) - ((3.0f/2.0f) * pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * y0 * (1- ((5*pow(z0,2))/pow(r,2)))) ;
                pvZ = (-(micro/(pow(r,3))) * z0) - ((3.0f/2.0f) * pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * z0 * (3- ((5*pow(z0,2))/pow(r,2)))) ;



                qx = Vx0 + (1.0f/2.0f) * h * pvX;
                qy = Vy0 + (1.0f/2.0f) * h * pvY;
                qz = Vz0 + (1.0f/2.0f) * h * pvZ;

                r =(pow((x0+ (1.0f/2.0f) * h * px),2)) + (pow((y0+ (1.0f/2.0f) * h * py),2)) + (pow((z0+ (1.0f/2.0f) * h * pz),2));
                r = sqrt(r);

                qvx = (-(micro/(pow(r,3))) * (x0+ (1.0f/2.0f) * h * px)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (x0+ (1.0f/2.0f) * h * px) * (1-((5*(pow((z0+0.5 * h * pz),2)))/pow(r,2)));
                qvy = (-(micro/(pow(r,3))) * (y0+ (1.0f/2.0f) * h * py)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (y0+ (1.0f/2.0f) * h * py) * (1-((5*(pow((z0+0.5 * h * pz),2)))/pow(r,2)));
                qvz = (-(micro/(pow(r,3))) * (z0+ (1.0f/2.0f) * h * pz)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (z0+ (1.0f/2.0f) * h * pz) * (3-((5*(pow((z0+0.5 * h * pz),2)))/pow(r,2)));

                rx = Vx0 + (1.0f/2.0f) * h * qvx;
                ry = Vy0 + (1.0f/2.0f) * h * qvy;
                rz = Vz0 + (1.0f/2.0f) * h * qvz;

                r =(pow((x0+ (1.0f/2.0f) * h * qx),2)) + (pow((y0+ (1.0f/2.0f) * h * qy),2)) + (pow((z0+ (1.0f/2.0f) * h * qz),2));
                r = sqrt(r);

                rvx = (-(micro/(pow(r,3))) * (x0+ (1.0f/2.0f) * h * qx)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (x0+ (1.0f/2.0f) * h * qx) * (1-((5*(pow((z0+0.5 * h * qz),2)))/pow(r,2)));
                rvy = (-(micro/(pow(r,3))) * (y0+ (1.0f/2.0f) * h * qy)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (y0+ (1.0f/2.0f) * h * qy) * (1-((5*(pow((z0+0.5 * h * qz),2)))/pow(r,2)));
                rvz = (-(micro/(pow(r,3))) * (z0+ (1.0f/2.0f) * h * qz)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (z0+ (1.0f/2.0f) * h * qz) * (3-((5*(pow((z0+0.5 * h * qz),2)))/pow(r,2)));


                sx = Vx0 + h * rvx;
                sy = Vy0 + h * rvy;
                sz = Vz0 + h * rvz;

                r =(pow((x0+ (1.0f/2.0f) * h * rx),2)) + (pow((y0+ (1.0f/2.0f) * h * ry),2)) + (pow((z0+ (1.0f/2.0f) * h * rz),2));
                r = sqrt(r);

                svx = (-(micro/(pow(r,3))) * (x0+ (1.0f/2.0f) * h * rx)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (x0+ (1.0f/2.0f) * h * rx) * (1-((5*(pow((z0+0.5 * h * rz),2)))/pow(r,2)));
                svy = (-(micro/(pow(r,3))) * (y0+ (1.0f/2.0f) * h * ry)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (y0+ (1.0f/2.0f) * h * ry) * (1-((5*(pow((z0+0.5 * h * rz),2)))/pow(r,2)));
                svz = (-(micro/(pow(r,3))) * (z0+ (1.0f/2.0f) * h * rz)) - (3.0f/2.0f)*pow(j,1) * ((micro*(pow(ae,2)))/(pow(r,5))) * (z0+ (1.0f/2.0f) * h * rz) * (1-((5*(pow((z0+0.5 * h * rz),2)))/pow(r,2)));


                /*final step*/

                x1 = x0 + (1.0f/6.0f) * h * (px + 2*qx + 2*rx + sx);
                y1 = y0 + (1.0f/6.0f) * h * (py + 2*qy + 2*ry + sy);
                z1 = z0 + (1.0f/6.0f) * h * (pz + 2*qz + 2*rz + sz);

                vx1 = Vx0 + (1.0f/6.0f) * h * (pvX + 2*qvx + 2*rvx + svx);
                vy1 = Vy0 + (1.0f/6.0f) * h * (pvY + 2*qvy + 2*rvy + svy);
                vz1 = Vz0 + (1.0f/6.0f) * h * (pvZ + 2*qvz + 2*rvz + svz);

                x0 = x1;
                y0 = y1;
                z0 = z1;

                Vx0 = vx1;
                Vy0 = vy1;
                Vz0 = vz1;
/*
                tmfv[0] = x1;
                tmfv[1] = y1;
                tmfv[2] = z1;
                tmfv[3] = vx1;
                tmfv[4] = vy1;
                tmfv[5] = vz1;
*/
            }


                c1 = y0 * Vz0 - z0 * Vy0;
                c2 = z0 * Vx0 - x0 * Vz0;
                c3 = x0 * Vy0 - y0 * Vx0;

                c = pow(c1,2) + pow(c2,2) + pow(c3,2);
                c = sqrt(c);

                l = atan2(-(c),c2);
                iTMF = acos(c3/c) ;
                u = acos((x0*cos(l) + y0*sin(l))/r);

                (*((orbitalTMFV+0*1)+0)) = l ;
                (*((orbitalTMFV+0*1)+1)) = iTMF ;
                (*((orbitalTMFV+0*1)+2)) = u ;

}

void TRIAD:: multiplyMatrices3x1(float *array0,float *array1, float *arrayOut)
{
    arrayOut[0] = (array0[0] * array1[0])+ (array0[1] * array1[1]) + (array0[2] * array1[2]);
    arrayOut[1] = (array0[3] * array1[0])+ (array0[4] * array1[1]) + (array0[5] * array1[2]);
    arrayOut[2] = (array0[6] * array1[0])+ (array0[7] * array1[1]) + (array0[8] * array1[2]);
}

void TRIAD::setAngleRM(int angleNr, float value)
{
    if(angleNr==2)
    {theta = value;}

    else if(angleNr==3)
    {epsilonClasse = value;}

    else
    {phi = value;}
}

void TRIAD::createFirstRotMatrix(float *firstRotMatrix)
{

    float phi=((getAngleRM(3))*PI)/180;
    float epsilon=((getAngleRM(1)) *PI)/180;
    float theta=((getAngleRM(2)) *PI)/180;
    qDebug("phi input: ");
    qDebug()<<getAngleRM(3)<<endl;
    qDebug("epsi input: ");
    qDebug()<<getAngleRM(1)<<endl;
    qDebug("theta input: ");
    qDebug()<<getAngleRM(2)<<endl;

    firstRotMatrix[0] = cos(phi) * cos(epsilon)  - cos(theta) * sin(phi) * sin(epsilon);
    firstRotMatrix[1] = cos(theta) * cos(epsilon) * sin(phi) + cos(phi) * sin(epsilon);
    firstRotMatrix[2] = sin(theta) * sin(phi);
    firstRotMatrix[3] = (-cos(epsilon)) * sin(phi) - cos(theta) * cos(phi) * sin(epsilon);
    firstRotMatrix[4] = cos(theta) * cos(phi) * cos(epsilon) - sin(phi) * sin(epsilon);
    firstRotMatrix[5] = cos(phi) * sin(theta);
    firstRotMatrix[6] = sin(theta) * sin(epsilon);
    firstRotMatrix[7] = (-cos(epsilon)) * sin(theta);
    firstRotMatrix[8] = cos(theta);

}

void TRIAD::workaroundMallocFail3x3(float *array0, float *array1, float output[3][3])
{

    output[0][0] = (array0[0] * array1[0]) + (array0[1] * array1[3]) + (array0[2] * array1[6]);
    output[1][0] = (array0[0] * array1[1]) + (array0[1] * array1[4]) + (array0[2] * array1[7]);
    output[2][0] = (array0[0] * array1[2]) + (array0[1] * array1[5]) + (array0[2] * array1[8]);

    output[0][1] = (array0[3] * array1[0]) + (array0[4] * array1[3]) + (array0[5] * array1[6]);
    output[1][1] = (array0[3] * array1[1]) + (array0[4] * array1[4]) + (array0[5] * array1[7]);
    output[2][1] = (array0[3] * array1[2]) + (array0[4] * array1[5]) + (array0[5] * array1[8]);

    output[0][2] = (array0[6] * array1[0]) + (array0[7] * array1[3]) + (array0[8] * array1[6]);
    output[1][2] = (array0[6] * array1[1]) + (array0[7] * array1[4]) + (array0[8] * array1[7]);
    output[2][2] = (array0[6] * array1[2]) + (array0[7] * array1[5]) + (array0[8] * array1[8]);
}

void TRIAD::transposeMatrix(float* array2, int height, int width, float * output, int height2, int width2)
{

    int i=0, j=0;

    for (i=0;i<height; i++)
    {
        for(j=0;j<width;j++)
        {
            output[j * height + i] = array2[i*width + j];

        }
    }

}

void TRIAD::multiplyMatrices(float *transposedMatrix, int height1, int width1, float *iSunVector,int height2, int width2, float *orbitalSunVector)
{
/*
    int y=0;
    int x = 0;
    int row=0;
    int column =0;
    int k=0;
    int k2=0;
    float result = 0;
    float *output2=NULL;

    int i=0;
    int j=0;

    i=0;
    k=0;
    k2 = 0;


    if(width1 != height2) //unmathcing matrices
    {
        return;
    }

    output2 = malloc(height1 * width2 * sizeof(float));


    while(k<width1) //aantal rijen 1ste matrix
    {
        for(j=0;j<height2;j++) //aantal rijen 2de matrix
        {
            result += (*((transposedMatrix+k*width1)+j)) * (*((iSunVector+j*width2)+k2));  //1ste var:aantal kolommen 2de matrix  --2de variabele na de plus = aantal kolommen 2de matrix
        }

        output2[row*width2 + column] = result;

        k2++;
        x++;
        column++;

        if(x==width2) //aantal kolommen 2de Matrix
        {
            k2=0;
            x=0;
            column=0;
            row++;
            y++;
            k++;
        }
        result = 0;

    }

    //tussenresultaat
    for(i=0;i<height1;i++)
    {
        for(j=0;j<width2;j++)
        {
            orbitalSunVector[j*height1 + i] = output2[i*width2 + j];

        }
    }

    free(output2);
*/
}

void TRIAD::multiplyCrossVectors2(float * first,float * second, float *output)
{
    int i;

    output[0] = (first[1] * second[2]) - (first[2] * second[1]);
    output[1] = (first[2] * second[0]) - (first[0] * second[2]);
    output[2] = (first[0] * second[1]) - (first[1] * second[0]);

    qDebug("in multiply func :");
    for(i=0;i<3;i++)
    {
        qDebug()<<first[i];
    }
    for(i=0;i<3;i++)
    {
        qDebug()<<second[i];
    }
    for(i=0;i<3;i++)
    {
        qDebug()<<output[i];
    }


}

void TRIAD::normalizeVector2(float *input, float *normalizedVec)
{
    float magnitude;

    magnitude = pow(input[0],2) + pow(input[1],2) + pow(input[2],2);
    magnitude = sqrt(magnitude);

    //printf("magnitude: %f\n", magnitude);
    normalizedVec[0]=input[0]/magnitude;
    normalizedVec[1]=input[1]/magnitude;
    normalizedVec[2]=input[2]/magnitude;

}

void TRIAD::regroupMatrix2(float *pi,  float *pi2, float *pi3,float *outputMatrix)
{
    outputMatrix[0] = pi[0];
    outputMatrix[1] = pi2[0];
    outputMatrix[2] = pi3[0];

    outputMatrix[3] = pi[1];
    outputMatrix[4] = pi2[1];
    outputMatrix[5] = pi3[1];

    outputMatrix[6] = pi[2];
    outputMatrix[7] = pi2[2];
    outputMatrix[8] = pi3[2];
}

void TRIAD::calcAnglesRM(float finalRotationMatrix[3][3])
{
    int i,j;
    float testTheta=0.0;
    float testPhi = 0.0;
    float testEpsi = 0.0;
    float test;

    testTheta = lround((acos(finalRotationMatrix[2][2])*180.0)/3.14);

    testEpsi = ((-(finalRotationMatrix[2][1]))/(sin((getAngleRM(2)*180)/PI)));
    testEpsi = (acos(testEpsi));
    testEpsi = (testEpsi) * (180/3.14);
    testEpsi = lround(testEpsi);

    testPhi = ((finalRotationMatrix[1][2])/(sin((getAngleRM(2)*180)/PI)));
    testPhi = (acos(testPhi))*180.0/3.14;
    testPhi = lround(testPhi);


    setAngleRM(2,testTheta);
    setAngleRM(1,testPhi);
    setAngleRM(3,testEpsi);

    qDebug("calculated angles final RM:");
    qDebug()<<testTheta<<endl;
    qDebug()<<testPhi<<endl;
    qDebug()<<testEpsi<<endl;

}



