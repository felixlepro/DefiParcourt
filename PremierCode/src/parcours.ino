/* ****************************************************************************
Projet:   Rob Bros.
Equipe:   P-23
Auteurs:  Felix-Antoine Amyot                     amyf2502
          Gabriel Cabana                          cabg2101
          Emile Charbonneau                       chae2424
          Santiago Sebastian Cosquillo Gutierrez  coss2803
          Mathis Gagnon                           gagm2354
          Samuel Grenier                          gres2117
          Philippe Lalonde                        lalp2803
          Olivier Roy                             royo2206

Description: Fonction qui permet de deplacer un robot sur un parcours preprogramme.
Date: 04-10-2018
**************************************************************************** */

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <math.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces

// Distance entre les deux roues du robot.
#define distance_between_wheels 18.65
#define INTER 50

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

int calculate_pulse_forward(int distance)
{
  /********
  Fonction qui calcule la valeur maximale de pulsation selon une distance fournie.
  Valeur fournie: distance (entier en centimetres).
  Valeur retournee: pulsation maximale (entier sans unites).
  ********/
  int max_pulse;
  max_pulse = int(distance*3200.0/(3.0*PI*2.54));
  return max_pulse;
}

int calculate_pulse_turn(int degree_angle)
{
  /********
  Fonction qui calcule la valeur maximale de pulsation selon un angle fourni.
  Valeur fournie: angle (entier en degres).
  Valeur retournee: pulsation maximale (entier sans unites).
  ********/
  int max_pulse_turn, distance;
  degree_angle %= 360;
  distance = float(distance_between_wheels/2*degree_angle*PI/180.0);
  max_pulse_turn = int(distance*3200.0/(3.0*PI*2.54));
  return max_pulse_turn;
}

float speed_correction(int encoder_left, int encoder_right, int t)
{
  /********
  Fonction qui calcule une correction de vitesse selon la pulsation de chaque roue.
  Valeurs fournies: pulsation de la roue gauche (entier sans unites);
                    pulsation de la roue droite (entier sans unites);
                    intervalle de temps (entier en millisecondes).
  Valeur retournee: correction de vitesse (nombre decimal en pourcentage).
  ********/
  float speed_variation, speed_offset, total_variation, coeff_total, coeff_speed;

  // Variation en distance et variation en vitesse.
  total_variation = float(encoder_left - encoder_right);
  speed_variation = float(total_variation/t);
  coeff_total = 50000.0;
  coeff_speed = 10000.0;
  speed_offset = total_variation/coeff_total + speed_variation/coeff_speed;
  return speed_offset;
}

int acceleration(int t, float limit_speed)
{
  /********
  Fonction qui change la vitesse des roues, de maniere a simuler une acceleration.
  Valeurs fournies: intervalle de temps (entier en millisecondes);
                    vitesse limite (nombre decimal en pourcentage).
  Valeur retournee: aucun retour.
  ********/
  float wheel_speed, speed_offset = 0.0;
  int i = 1;
  for(wheel_speed = limit_speed/10; wheel_speed <= limit_speed; wheel_speed += limit_speed/10)
  {
    // Boucle de vitesse des moteurs.
    MOTOR_SetSpeed(0, wheel_speed);
    MOTOR_SetSpeed(1, wheel_speed + speed_offset);

    // Correction de la vitesse du moteur droit.
    speed_offset = speed_correction(ENCODER_Read(0), ENCODER_Read(1), i*t);
    i++;
    delay(t);
  }
  return ENCODER_Read(0);
}

void decceleration(int t, int i, float limit_speed)
{
  /********
  Fonction qui change la vitesse des roues, de maniere a simuler une acceleration.
  Valeurs fournies: intervalle de temps (entier en millisecondes);
                    vitesse limite (nombre decimal en pourcentage).
  Valeur retournee: aucun retour.
  ********/
  float wheel_speed, speed_offset = 0.0;
  for(wheel_speed = limit_speed; wheel_speed >= 0; wheel_speed -= limit_speed/10)
  {
    // Boucle de vitesse des moteurs.
    MOTOR_SetSpeed(0, wheel_speed);
    MOTOR_SetSpeed(1, wheel_speed + speed_offset);

    // Correction de la vitesse du moteur droit.
    //speed_offset = speed_correction(ENCODER_Read(0), ENCODER_Read(1), i*t);
    i++;
    delay(t);
  }
}

void forward(int distance, float wheel_speed, int t)
{
  /********
  Fonction qui fait avancer le robot en ligne droite.
  Valeurs fournies: distance a parcourir (entier en centimetres);
                    vitesse limite (nombre decimal en pourcentage);
                    intervalle de temps (entier en millisecondes).
  Valeur retournee: aucun retour.
  ********/
  int max_pulse_forward, acc_pulse;
  max_pulse_forward = calculate_pulse_forward(distance);

  // Acceleration lineaire de l'arret a la vitesse maximale.
  acc_pulse = acceleration(2*t, wheel_speed);

  if(acc_pulse < max_pulse_forward)
  {
    // Demarrage des moteurs.
    MOTOR_SetSpeed(0, wheel_speed);
    MOTOR_SetSpeed(1, wheel_speed);

    int i = 11;
    float new_wheel_speed;
    while(ENCODER_Read(1) < max_pulse_forward - 2*acc_pulse)
    {
      // Correction de la vitesse du moteur droit.
      delay(t);
      new_wheel_speed = wheel_speed + speed_correction(ENCODER_Read(0), ENCODER_Read(1), i*t);
      MOTOR_SetSpeed(0, new_wheel_speed);
      i++;

      // Impression des valeurs des encodeurs et de la pulsation maximale.
      Serial.println(ENCODER_Read(0));
      Serial.println(ENCODER_Read(1));
      Serial.println(max_pulse_forward);
    }

    // Decceleration lineaire de la vitesse maximale a l'arret.
    decceleration(2*t, i, wheel_speed);
  }

  // Arret des moteurs et reinitialisation des encodeurs
  MOTOR_SetSpeed(0,0.0);
  MOTOR_SetSpeed(1,0.0);
  delay(200);
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

void turn(int degree_angle, float wheel_speed, int bool_value)
{
  /********
  Fonction qui fait tourner le robot sur lui-meme.
  Valeurs fournies: angle de rotation (entier en degres);
                    vitesse limite (nombre decimal en pourcentage);
                    valeur booleenne (entier sans unites).
  Valeur retournee: aucun retour.
  ********/
  int max_pulse_turn;
  max_pulse_turn = calculate_pulse_turn(degree_angle);
  
  // Correction de la vitesse du moteur gauche.
  if((bool_value == 0))
    max_pulse_turn /= 1.03;

  while(ENCODER_Read(bool_value) < max_pulse_turn)
  {
    // Demarrage des moteurs.
    if((bool_value == 1))
    {
      // Moteur de droite tourne vers l'avant (virage a gauche).
      MOTOR_SetSpeed(0, -wheel_speed);
      MOTOR_SetSpeed(1, wheel_speed);
    }
    if((bool_value == 0))
    {
      // Moteur de gauche tourne vers l'avant (virage a droite).
      MOTOR_SetSpeed(0, wheel_speed);
      MOTOR_SetSpeed(1, -wheel_speed);
    }
    // Impression des valeurs des encodeurs et de la pulsation maximale.
    Serial.println(ENCODER_Read(0));
    Serial.println(ENCODER_Read(1));
    Serial.println(max_pulse_turn);
  }
  // Arret des moteurs et reinitialisation des encodeurs
  MOTOR_SetSpeed(0,0.0);
  MOTOR_SetSpeed(1,0.0);
  delay(200);
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les variables globales

void setup()
{
  BoardInit();
}


/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop()
{
    // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(10);// Delais pour decharger le CPU
  if((ROBUS_IsBumper(3)))
  {
    // Parcours (aller).
    forward(237, 0.6, INTER);
    turn(85, 0.3, 1);
    forward(55, 0.5, INTER);
    turn(92, 0.3, 0);
    forward(50, 0.5, INTER);
    turn(92, 0.3, 0);
    forward(54, 0.5, INTER);
    turn(85, 0.3, 1);
    forward(18, 0.5, INTER);
    turn(44, 0.3, 0);
    forward(50, 0.5, INTER);
    turn(90, 0.3, 1);
    forward(72, 0.5, INTER);
    turn(45, 0.3, 0);
    forward(50, 0.5, INTER);
    turn(14, 0.3, 0);
    forward(105, 0.5, INTER);
    
    turn(185, 0.3, 0);

    // Parcours (retour)
    forward(105, 0.5, INTER);
    turn(14, 0.3, 1);
    forward(50, 0.5, INTER);
    turn(41, 0.3, 1);
    forward(70, 0.5, INTER);
    turn(90, 0.3, 0);
    forward(50, 0.5, INTER);
    turn(42, 0.3, 1);
    forward(18, 0.5, INTER);
    turn(92, 0.3, 0);
    forward(54, 0.5, INTER);
    turn(85, 0.3, 1);
    forward(50, 0.5, INTER);
    turn(85, 0.3, 1);
    forward(55, 0.5, INTER);
    turn(92, 0.3, 0);
    forward(244, 0.6, INTER);
    
  }
}