/*
 * Nom du programme: 
 * 
 * La description: 
 * Un démarreur STAR-DELTA basé sur Arduino pour un moteur à induction triphasé.
 * Il y a trois entrées, une entrée de bouton de démarrage, une entrée de bouton d'arrêt et une adéquation de l'alimentation électrique pour exécuter la vérification intégrée.
 * Le programme contrôle trois contacteurs triphasés pour séquencer leur commutation dans le bon ordre pour permettre un bon démarrage.
 * REMARQUE: La configuration du câblage STAR-DELTA n'est pas effectuée dans le logiciel, elle doit donc être effectuée en externe.
 * 
 * Date: Jul-27-2020
 * 
 * Par: Menad et Has.
 * 
 * Version: 1.0 (beta)
 * 
 */


// Input - Output Affectations

const uint8_t COMMENCE = 2;
const uint8_t ARRET = 3;

const uint8_t M1 = 4;
const uint8_t M2 = 5;
const uint8_t M3 = 6;

const uint8_t S1 = 7;
const uint8_t S2 = 8;
const uint8_t S3 = 9;

const uint8_t D1 = 10;
const uint8_t D2 = 11;
const uint8_t D3 = 12;

const uint8_t Source_de_Courant = 13;

// Affectations des délais Timinig

const int Star_a_Principal_interval = 50; // Time interval between when Star contacteur is closed to when Main contacteur is closed
const int Dure_en_debut_STAR = 6000; // Duration motor will COMMENCE in Star configuration.
const int Star_a_Delta_temporisation = 50; // Star to Delta transition time
unsigned long Current_millis = 0;
unsigned long Previous_millis = 0;

// Flags

bool demarrerLanceDrapeau = false;
bool arretInitieDrapeau = false;

bool LaSourceDAlimentationDuMoteurEstBonne = false;
bool contacteurPrincipalPretADemarrer = false;
bool contacteurStarPretADemarrer = false;
bool contacteurDeltaPretADemarrer = false;

bool pretACommencer = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing Arduino Uno Based Star - Delta COMMENCEer Program");

  pinMode(Source_de_Courant, INPUT);
  
  initialiserLesBoutonsDEntree();
  initialiserLeContacteurPRINCIPAL();
  initialiserLeContacteurSTAR();
  initialiserLeContacteurDELTA();
  Serial.println("Initialisation terminée bien!");
  delay(2000);    // You can take this delay out if you want.
}

void loop() {
  Serial.println("Entré dans la boucle principale, en attente de l'entrée de l'utilisateur...");
  laRoutineACommencer();
  delay(3000);              // Ce délai est de ralentir l'exécution du programme pour nous permettre de suivre la séquence du programme lors du dépannage ou de la vérification du code. Vous pouvez le supprimer dans la version finale. Vous pouvez utiliser cette technique lors du développement de code.
}

// Subroutine functions

void initialiserLesBoutonsDEntree()
{
  pinMode(COMMENCE, INPUT);
  digitalWrite(COMMENCE, LOW);
  attachInterrupt(digitalPinToInterrupt(COMMENCE), boutonDeDemarrageEnfonce, RISING);   // Pourquoi est-ce "Rising"?
  pinMode(ARRET, INPUT);
  digitalWrite(ARRET, HIGH);
  attachInterrupt(digitalPinToInterrupt(ARRET), boutonDAarretEnfonce, FALLING);   // Pourquoi est-ce "Falling"?
  Serial.println("Initialisation du bouton d'entrée terminée bien!");
  Serial.print("L'état de départ est actuellement = ");
  Serial.println(digitalRead(COMMENCE));
  Serial.print("L'état d'arrêt est actuellement = ");
  Serial.println(digitalRead(COMMENCE));
  Serial.println();
  Serial.println();
}

void initialiserLeContacteurPRINCIPAL()
{
  pinMode(M1, OUTPUT);
  digitalWrite(M1, LOW);
  pinMode(M2, OUTPUT);
  digitalWrite(M2, LOW);
  pinMode(M3, OUTPUT);
  digitalWrite(M3, LOW);
  Serial.println("Initialisation du contacteur PRINCIPAL terminée bien!");
  Serial.print("M1 = ");
  Serial.print(digitalRead(M1));
  Serial.print("\t");               // Notez que j'ai utilisé des onglets au lieu de nouvelles lignes ici. Juste pour te montrer à quoi ça ressemble.
  Serial.print("M2 = ");
  Serial.print(digitalRead(M2));
  Serial.print("\t");
  Serial.print("M3 = ");
  Serial.print(digitalRead(M3));
  Serial.println();
  Serial.println();
}

void initialiserLeContacteurSTAR()
{
  pinMode(S1, OUTPUT);
  digitalWrite(S1, LOW);
  pinMode(S2, OUTPUT);
  digitalWrite(S2, LOW);
  pinMode(S3, OUTPUT);
  digitalWrite(S3, LOW);
  Serial.println("Initialisation du contacteur STAR terminée bien!");
  Serial.print("S1 = ");
  Serial.println(digitalRead(S1));
  Serial.print("S2 = ");
  Serial.println(digitalRead(S2));
  Serial.print("S3 = ");
  Serial.println(digitalRead(S3));
  Serial.println();
  Serial.println();
}

void initialiserLeContacteurDELTA()
{
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);
  Serial.println("Initialisation du contacteur DELTA terminée bien!");
  Serial.print("D1 = ");
  Serial.println(digitalRead(D1));
  Serial.print("D2 = ");
  Serial.println(digitalRead(D2));
  Serial.print("D3 = ");
  Serial.println(digitalRead(D3));
  Serial.println();
  Serial.println();
}

void boutonDeDemarrageEnfonce()
{
  Serial.println("Bouton de démarrage enfoncé!");
  Serial.println();
  demarrerLanceDrapeau = true;
  //demarrerLanceDrapeau != demarrerLanceDrapeau;
  if(demarrerLanceDrapeau)
  {
    Serial.println("Appel de la routine de démarrage...");
    Serial.println();
    //laRoutineACommencer();
  }
  else
  {
    Serial.println("Bouton de démarrage déjà enfoncé, le démarrage est déjà en cours...");
    Serial.println();
  } 
}

void boutonDAarretEnfonce()
{
  Serial.println("Le bouton ARRET a été enfoncé!");
  Serial.println();
  arretInitieDrapeau = true;
  demarrerLanceDrapeau = false;
  Serial.println("Appel de la routine d'arrêt...");
  Serial.println();
  laRoutineAAarreter();
}



bool verifieSiVousEtesPretACommencer()
{
  ouvertLeContacteurSTAR();   // Premièrement, ouvrez tous les contacteurs pour réinitialiser / initialiser la séquence de démarrage.
  ouvertLeContacteurPRINCIPAL();
  ouvertLeContacteurDELTA();  // Tous les contacteurs doivent maintenant être en position ouverte, prêts à démarrer.
  
  bool Source_de_CourantGood = verificationDeLAlimentation();

  bool mainContacteurRTS = VerifiezLeContacteurPRINCIPAL();

  bool starContacteurRTS = VerifiezLeContacteurSTAR();

  bool deltaContacteurRTS = VerifiezLeContacteurDELTA();

  /*
  if(digitalRead(Source_de_Courant) == 1)
  {
    Serial.println("Power Supply checked good!");
    LaSourceDAlimentationDuMoteurEstBonne = true;
  }
  else
  {
    Serial.println("No supply voltage for COMMENCEing. Please check power supply connections.");
    LaSourceDAlimentationDuMoteurEstBonne = false;
  }
  
  if(digitalRead(M1) == 0 && digitalRead(M2) == 0 && digitalRead(M3) == 0)
  {
    contacteurPrincipalPretADemarrer = true;
    Serial.println("Main contacteur is ready to COMMENCE.");
  }
  else
  {
    contacteurPrincipalPretADemarrer = false;
    Serial.println("Main contacteur is NOT ready to COMMENCE.");
  }

  if(digitalRead(S1) == 0 && digitalRead(S2) == 0 && digitalRead(S3) == 0)
  {
    contacteurStarPretADemarrer = true;
    Serial.println("Star contacteur is ready to COMMENCE.");
  }
  else
  {
    contacteurStarPretADemarrer = false;
    Serial.println("Star contacteur is NOT ready to COMMENCE.");
  }

  if(digitalRead(D1) == 0 && digitalRead(D2) == 0 && digitalRead(D3) == 0)
  {
    contacteurDeltaPretADemarrer = true;
    Serial.println("Delta contacteur is ready to COMMENCE.");
  }
  else
  {
    contacteurDeltaPretADemarrer = false;
    Serial.println("Delta contacteur is NOT ready to COMMENCE.");
  } */

  if(Source_de_CourantGood && mainContacteurRTS && starContacteurRTS && deltaContacteurRTS)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

bool verificationDeLAlimentation()   // Check if motor supply voltage is good
{
  if(digitalRead(Source_de_Courant) == 1)
  {
    Serial.println("L'alimentation a été vérifiée et est bonne!");
    LaSourceDAlimentationDuMoteurEstBonne = true;      // Set flag
    return LaSourceDAlimentationDuMoteurEstBonne;
  }
  else
  {
    Serial.println("Pas de tension d'alimentation pour le démarrage. Veuillez vérifier les connexions d'alimentation.");
    LaSourceDAlimentationDuMoteurEstBonne = false;
    return LaSourceDAlimentationDuMoteurEstBonne;    // Reset flag
  }
}

bool VerifiezLeContacteurPRINCIPAL()    // Check if main contacteur is off and not already on.
{
  if(digitalRead(M1) == 0 && digitalRead(M2) == 0 && digitalRead(M3) == 0)
  {
    contacteurPrincipalPretADemarrer = true;
    Serial.println("Le contacteur principal est prêt à démarrer.");
    return contacteurPrincipalPretADemarrer;
  }
  else
  {
    contacteurPrincipalPretADemarrer = false;
    Serial.println("Le contacteur principal n'est PAS prêt à démarrer.");
    return contacteurPrincipalPretADemarrer;
  }
}

bool VerifiezLeContacteurSTAR()     // Check if star contacteur is off and not already on.
{
  if(digitalRead(S1) == 0 && digitalRead(S2) == 0 && digitalRead(S3) == 0)
  {
    contacteurStarPretADemarrer = true;
    Serial.println("Star contacteur est prêt à démarrer.");
    return contacteurStarPretADemarrer;
  }
  else
  {
    contacteurStarPretADemarrer = false;
    Serial.println("Star contacteur n'est PAS prêt à démarrer.");
    return contacteurStarPretADemarrer;
  }
}

bool VerifiezLeContacteurDELTA()     // Check if delta contacteur is off and not already on.
{
   if(digitalRead(D1) == 0 && digitalRead(D2) == 0 && digitalRead(D3) == 0)
  {
    contacteurDeltaPretADemarrer = true;
    Serial.println("Le contacteur Delta est prêt à démarrer.");
    return contacteurDeltaPretADemarrer;
  }
  else
  {
    contacteurDeltaPretADemarrer = false;
    Serial.println("Le contacteur Delta n'est PAS prêt à démarrer.");
    return contacteurDeltaPretADemarrer;
  }
}

void laRoutineACommencer()
{
  Serial.println("Nous sommes entrés dans la fonction principale....");

  if(demarrerLanceDrapeau)
  {
    pretACommencer = verifieSiVousEtesPretACommencer();

    if(pretACommencer)
    {
      Serial.println("Démarrage...");    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      Serial.println();                 // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      
      Previous_millis = millis();       // Réinitialiser la minuterie de comptage.
      Serial.print("Prev mils = ");     // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      Serial.println(Previous_millis);   // Pour le dépannage uniquement. Peut être supprimé dans la version finale.

      Current_millis = millis();
      Serial.print("Curr mils = ");     // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      Serial.println(Current_millis);   // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
        
      while(Current_millis - Previous_millis <= Star_a_Principal_interval)
      {
        Serial.print("P = ");               // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
        Serial.println(Previous_millis);    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
    
        Current_millis = millis();    // Continuez à mettre à jour le temps écoulé jusqu'à ce que la durée du délai soit atteinte, afin que nous puissions sortir de la boucle.
    
        Serial.print("C = ");             // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
        Serial.println(Current_millis);   // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
    
        unsigned long diff = Current_millis - Previous_millis;    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
    
        Serial.print("Diff = ");    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
        Serial.println(diff);       // Pour le dépannage uniquement. Peut être supprimé dans la version finale.

        fermerLeContacteurSTAR();   // Cela peut être fait en dehors de la boucle "while" si cela pose des problèmes. Si c'est le cas, placez-le juste avant le début de la boucle.
        Serial.println("Le contacteur STAR a été fermé avec succès!.........................................");    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      }
      
      delay(3000);    // Ce délai ne sert qu'à ralentir l'exécution du programme pour permettre le dépannage si nécessaire au cours de la séquence d'étapes. You can delete this.
      Serial.println("Passer à l'étape suivante de la séquence de démarrage après la boucle................................................."); // Cette instruction d'impression est également pour aider à observer le bon déroulement / séquence du programme. Cela peut être supprimé dans la version finale.
  
      // Vérifions à nouveau pour nous assurer que les conditions de démarrage sont toujours bonnes.
      LaSourceDAlimentationDuMoteurEstBonne = verificationDeLAlimentation();     
      contacteurPrincipalPretADemarrer = VerifiezLeContacteurPRINCIPAL();
      contacteurDeltaPretADemarrer = VerifiezLeContacteurDELTA();

      Previous_millis = millis();    // Réinitialiser la minuterie de comptage, pour le prochain calcul de retard.
      Current_millis = millis();
      
      while((Current_millis - Previous_millis <= Dure_en_debut_STAR) && LaSourceDAlimentationDuMoteurEstBonne && contacteurPrincipalPretADemarrer && contacteurDeltaPretADemarrer)
      {
        // N'oubliez pas que le contacteur STAR est toujours fermé à ce stade.
        
        Current_millis = millis();

        fermerLeContacteurPRINCIPAL();  // Vous devez terminer l'écriture de la fonction "fermerLeContacteurPRINCIPAL ()" comme la fonction "fermerLeContacteurSTAR ()"
        
        Serial.println("Le contacteur principal fermé et démarrant en STAR!.........................................");      // Pour le dépannage uniquement. Peut être supprimé dans la version finale.
      }

      delay(2000);    // Pour le dépannage uniquement. Peut être supprimé dans la version finale.

      Previous_millis = millis();    // Réinitialiser la minuterie de comptage.
      Current_millis = millis();
     
      while((Current_millis - Previous_millis <= Star_a_Delta_temporisation) && LaSourceDAlimentationDuMoteurEstBonne && contacteurDeltaPretADemarrer)
      {
        
        Current_millis = millis();
        
        ouvertLeContacteurSTAR();
        
        contacteurStarPretADemarrer = VerifiezLeContacteurSTAR();   // Assurez-vous que le contacteur de démarrage est ouvert.
        Serial.println("Contacteur STAR ouvert avec succès.............!");
        Serial.println();
      }

      delay(2000); // Pour le dépannage uniquement. Peut être supprimé dans la version finale.

      Current_millis = millis();
   
      if((Current_millis - Previous_millis > Star_a_Delta_temporisation) && LaSourceDAlimentationDuMoteurEstBonne && contacteurDeltaPretADemarrer && contacteurStarPretADemarrer)
      {
        fermerLeContacteurDELTA();
        Serial.println("Courir dans Delta!");
        Serial.println();
        delay(5000);
        demarrerLanceDrapeau = false;
        Serial.println("Séquence de démarrage terminée. Prêt pour une autre entrée de démarrage.");
      }
    }
  } 
}

void laRoutineAAarreter()
{
  ouvertLeContacteurPRINCIPAL();
  ouvertLeContacteurDELTA();
  ouvertLeContacteurSTAR();
}

void fermerLeContacteurSTAR()    // Cette fonction permet de regrouper le code utilisé pour fermer ou mettre sous tension le contacteur.
{
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  Serial.println("Star Contacteur Closed!");
  Serial.print("S1 = ");
  Serial.print(digitalRead(S1));
  Serial.print("\t");
  Serial.print("S2 = ");
  Serial.print(digitalRead(S2));
  Serial.print("\t");
  Serial.print("S3 = ");
  Serial.print(digitalRead(S3));
  Serial.println();
}

void fermerLeContacteurPRINCIPAL() // Vous devez terminer l'écriture de la fonction "fermerLeContacteurPRINCIPAL ()" comme la fonction "fermerLeContacteurSTAR ()"
{
  
}

void fermerLeContacteurDELTA()    // Comme above function, vous devez terminer l'écriture de cette fonction comme l'exemple de fonction pour fermer "STAR".
{
  
}

void ouvertLeContacteurSTAR()   // Cette fonction consiste à déclencher le contacteur.
{
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Serial.print("S1 = ");
  Serial.print(digitalRead(S1));
  Serial.print("\t");
  Serial.print("S2 = ");
  Serial.print(digitalRead(S2));
  Serial.print("\t");
  Serial.print("S3 = ");
  Serial.print(digitalRead(S3));
  Serial.println();
}

void ouvertLeContacteurPRINCIPAL()    // C'est à vous de terminer en utilisant l'exemple ci-dessus.
{
  
}

void ouvertLeContacteurDELTA()    // C'est à vous de terminer en utilisant l'exemple ci-dessus.
{
  
}
