#include <Arduino.h>
#include <LibRobus.h>


void avancer(float distance,float vitesse = 0.5)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    // limite de 2 m
    distance = 3200 *distance/0.25;// Encodeur tour//Diametre roue en m
    vitesse *= 0.95;
    float vitesseCorriger = vitesse;
    float correction;
    int x = 0;
    int g = 0,dr =0 ;

    float p = 0.00001;//0.000015;
    float i = 800000;
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
        correction += sommeErreur / i;
        //Serial.println(correction);
        //correction += (erreur - erreurPrec) / d;
        //correction /= 1000;
        vitesseCorriger += correction;

        MOTOR_SetSpeed(1,vitesseCorriger);
        x = g;
        
        Serial.println(vitesseCorriger);
        sommeErreur += erreur;
        //erreurPrec = erreur;
        
    }Serial.println(x);
        Serial.println(dr);
}
void tourner(float angle,float vitesse,float rayon =0)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    float arcInt = PI * rayon / 180 * angle;
    float arcExt = PI * (rayon + 0.192) / 180 * angle;//distance entre roues (m)

    int inter = 1, ext = 0;

    float distInt = abs(3200 *arcInt/0.25);// Encodeur tour//Diametre roue en m
    float distExt = abs(3200 *arcExt/0.25);// Encodeur tour//Diametre roue en m
    float parcouruExt;
    float parcouruInt;
    
    float p = 5;
    float i = 0;
    float d = 0;

    float erreur = 0;
    float sommeErreur = 0;
    float erreurPrec = 0;

    float ratio = distExt/distInt;
    float ratioReel = ratio;
    float ratioCorrection = ratio;

    if (angle < 0)
    {
        inter = 0;
        ext = 1;
    }
    vitesse *= .90; 
    float vitesseCorriger = vitesse;
    vitesse = vitesseCorriger/ratio;
    Serial.println(ratio);
    MOTOR_SetSpeed(inter,vitesse);
    MOTOR_SetSpeed(ext, vitesseCorriger);
    
    while (distExt > parcouruExt)
    {
        delay(50);
        parcouruExt = ENCODER_Read(ext);
        parcouruInt = ENCODER_Read(inter);
        if(p != 0)
        {
            ratioReel = parcouruExt/parcouruInt;
            erreur = (ratio - ratioReel);
        }
        if(parcouruInt>=distInt)
        {
            MOTOR_SetSpeed(inter,0);
        }

        
        ratioCorrection += erreur / p;
        //correction += sommeErreur *i;
        //correction += (erreur - erreurPrec) * d;

        vitesseCorriger = vitesse * ratioCorrection;

        //Serial.println(ratioReel);
        Serial.println(ratioCorrection);
        MOTOR_SetSpeed(ext,vitesseCorriger);

        erreurPrec = erreur;
        sommeErreur += erreur;
    }
    Serial.println(ENCODER_Read(0));
    Serial.println(distExt);
}
float acceleration(float speedFinal)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);

    float g,dr,correction,erreur,sommeErreur,p = 1/1,i=1000000,d=1000,vitesseCorriger,erreurPrec = 0;
    float speed = 0.1;
    MOTOR_SetSpeed(0,speed);
    MOTOR_SetSpeed(1,speed);
    
    while (speed < speedFinal)
    {
        delay(100);
        speed += .015;

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
        //Serial.println(correction);
        sommeErreur += erreur;
        erreurPrec = erreur;
    }
    MOTOR_SetSpeed(0,speedFinal);
    float distanceFinale = ENCODER_Read(1)/3200*.25;
    Serial.println(distanceFinale);
    Serial.print(g);
    Serial.println(dr);
    return distanceFinale;
}
void stop()
{
    Serial.println("stop");
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
}
void uTurn(float speed)
{
    MOTOR_SetSpeed(0,-speed);
    MOTOR_SetSpeed(1,speed);
    delay(930);
}
void setup() {
    // put your setup code here, to run once:
    BoardInit();
    while(!ROBUS_IsBumper(3))
    {

    }
}
void loop() {
    //acceleration(.5);
    //avancer(2,.4);
    stop();
    while(!ROBUS_IsBumper(3))
    {

    }
    //uTurn(0.4);
    //tourner(180,.6,.129);
    stop();
    delay(1000);
    //stop();
    //tourner(45,.6,.129);
    // Parcourt

    float speed = 0.4f;
    float rayon = 0.129;
    avancer(1.9-acceleration(speed),speed);
    tourner(-80,speed,rayon);
    avancer(0.1,speed);
    tourner(180,speed,rayon);
    avancer(0.05,speed);
    tourner(-95,speed,rayon);
    stop();
    tourner(37,speed,rayon);
    avancer(0.25,speed);
    tourner(-95,speed,rayon);
    avancer(.50,speed);
    tourner(30,speed,rayon);
    avancer(0.30,speed);
    tourner(13.5,speed,rayon);
    avancer(.78,speed);

    // U turn
    uTurn(speed);
    stop();
    delay(300);
    // Retour
    avancer(0.78,speed);
    tourner(-15,speed,rayon);
    avancer(0.30,speed);
    tourner(-35,speed,rayon);
    avancer(.50,speed);
    tourner(85,speed,rayon);
    avancer(0.20,speed);
    tourner(-50,speed,rayon);
    stop();
    tourner(80,speed,rayon);
    avancer(0.15,speed);
    tourner(-180,speed,rayon);
    avancer(0.05,speed);
    tourner(110,speed,rayon);
    avancer(2.5,speed); 
}

