#include <Arduino.h>
#include <LibRobus.h>
//int x = 0;


void avancer(float distance,float vitesse = 0.5)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    // limite de 2 m
    distance = 3200 *distance/0.24;// Encodeur tour//Diametre roue en m
    vitesse *= 0.95;
    float vitesseCorriger = vitesse;
    float correction;
    int x = 0;
    int g = 0,dr =0 ;

    float p = 0.00001;
    float i = 100;
    float d = 5;

    float erreur = 0;
    float sommeErreur = 0;
    float erreurPrec = 0;


    MOTOR_SetSpeed(0,vitesse);
    MOTOR_SetSpeed(1,vitesse);
    
    while (x<distance)
    {
        delay(100);
        g = ENCODER_Read(0);
        dr = ENCODER_Read(1);

        erreur = g-dr;
        correction = 0;
        correction += erreur*p;
        //correction += sommeErreur / i;
        //correction += (erreur - erreurPrec) / d;
        //correction /= 1000;
        vitesseCorriger += correction;

        MOTOR_SetSpeed(1,vitesseCorriger);
        x = g;
        
        //Serial.println(vitesseCorriger);
        //sommeErreur += erreur;
        //erreurPrec = erreur;
        
    }Serial.println(x);
        Serial.println(dr);
}
void tourner(float angle,float vitesse,float rayon)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    vitesse *= .90; 
    float vitesseCorriger = vitesse;
    float arcInt = PI * rayon / 180 * angle;
    float arcExt = PI * (rayon + 0.1865) / 180 * angle;//distance entre roues (m)

    int inter = 1, ext = 0;

    float distInt = abs(3200 *arcInt/0.22);// Encodeur tour//Diametre roue en m
    float distExt = abs(3200 *arcExt/0.237);// Encodeur tour//Diametre roue en m
    float parcouruExt;
    float parcouruInt;
    
    float p = 10;
    float i = 0;
    float d = 0;

    float erreur = 0;
    float sommeErreur = 0;
    float erreurPrec = 0;

    float ratio = distExt/distInt;
    float ratioReel = ratio;
    float correction;

    if (angle < 0)
    {
        inter = 0;
        ext = 1;
    }
    vitesseCorriger *= ratio;
    MOTOR_SetSpeed(inter,vitesse);
    MOTOR_SetSpeed(ext, vitesseCorriger);
    
    while (distExt > parcouruExt)
    {
        delay(100);
        parcouruExt = ENCODER_Read(ext);
        parcouruInt = ENCODER_Read(inter);
        ratioReel = parcouruExt/parcouruInt;

        if(parcouruInt>distInt)
        {
            MOTOR_SetSpeed(inter,0);
        }

        erreur = (ratio - ratioReel);
        correction = 0;
        correction += erreur / p;
        correction += sommeErreur *i;
        correction += (erreur - erreurPrec) * d;

        vitesseCorriger = vitesseCorriger + correction;

        Serial.println(erreur);
        Serial.println(correction);
        MOTOR_SetSpeed(ext,vitesseCorriger);

        erreurPrec = erreur;
        sommeErreur += erreur;
    }
}
float acceleration(float speedFinal)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);

    float g,dr,correction,erreur,sommeErreur,p = 10,i=1000000,d=1000,vitesseCorriger,erreurPrec = 0;
    float speed = 0.1;
    MOTOR_SetSpeed(0,speed);
    MOTOR_SetSpeed(1,speed);
    delay(300);
    while (speed < speedFinal)
    {
        delay(20);
        speed += .003;

        g = ENCODER_Read(0);
        dr = ENCODER_Read(1);

        erreur = g-dr;
        correction = 0;
        correction += erreur*p;
        correction += sommeErreur / i;
        //correction += (erreur - erreurPrec) / d;
        correction /= 1000;
        vitesseCorriger += correction;
        MOTOR_SetSpeed(0,speed);
        MOTOR_SetSpeed(1,speed+correction);
        Serial.println(correction);
        sommeErreur += erreur;
        erreurPrec = erreur;
    }
    MOTOR_SetSpeed(0,speedFinal);
    float distanceFinale = ENCODER_Read(1)*3200/.24;
    return distanceFinale;
}
void stop()
{
    Serial.println("stop");
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
}
void setup() {
    // put your setup code here, to run once:
    BoardInit();
}
void loop() {
    //acceleration(.5);
    avancer(2,.5);
    stop();
    //tourner(-180,.2,.1);
    stop();
    delay(1000);
    //stop();
    //delay(1000);
    /*Serial.println(ENCODER_ReadReset(0));
    Serial.println(ENCODER_ReadReset(1));*/
   // x++;
}

