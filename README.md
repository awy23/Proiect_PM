# Stație meteo cu afișaj și alertă sonoră

Vasile Andrei-Alexandru 333CB

# Introducere
Proiectul consta intr-o statie meteo care masoara temperatura si umiditatea mediului inconjurator, afisand valorile pe un display LCD si generand alerte sonore printr-un buzzer in functie de pragurile de temperatura.

Scopul proiectului este de a crea un sistem simplu si eficient de monitorizare ambientala, util in spatii inchise sau exterioare, care poate avertiza utilizatorul atunci cand conditiile depasesc anumite limite, fara a necesita supraveghere permanenta.

Ideea a pornit de la nevoia unui dispozitiv accesibil care sa ofere informatii esentiale despre mediul ambiant si sa semnaleze automat conditii potential nefavorabile.

Consider ca proiectul este valoros din punct de vedere educational, oferind experienta practica in domeniul senzorilor, al microcontrolerelor si al interfatarii hardware-software. De asemenea, poate fi extins si adaptat pentru aplicatii precum monitorizarea serelor, camerelor tehnice sau altor medii sensibile la variatii de temperatura si umiditate.

# Descriere generala
Proiectul este o statie meteo simpla si eficienta, destinata monitorizarii conditiilor de mediu. Sistemul masoara temperatura si umiditatea din aer folosind un senzor DHT11 si afiseaza informatiile pe un ecran LCD controlat prin interfata I2C. In plus, integreaza un buzzer care ofera alerta sonora daca temperatura depaseste un prag prestabilit, permitand utilizatorului sa ia masuri rapide.

Datele sunt colectate si procesate cu ajutorul unei placi Arduino Uno R3, iar toate componentele sunt conectate pe o placa breadboard pentru usurinta in asamblare si testare. Proiectul este potrivit pentru monitorizarea simpla a mediului in spatii inchise si poate fi extins pentru a include mai multe functii sau senzori suplimentari.

# Hardware design

Lista de piese:
-	Arduino Uno R3	(Placa principala de control)
-	Senzor DHT11	(Senzor temperatura si umiditate)
-	Display LCD 16x2 I2C	(Modul de afisare a valorilor)
- Buzzer	(Emitere alerta sonora)
-	Breadboard	(Platforma pentru conexiuni rapide)
-	Fire jumper 	(Conectare intre componente)

