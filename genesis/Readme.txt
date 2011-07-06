Versione	0.3

Si ringrazia manfredo per l'idea di come fare le colonne nella stanza centrale e per altre piccole cose :)

- Comandi
	ESC = esce dal programma
	F1 = passaggio da modalità window a fullscreen
	F2 = risoluzione a 640x480
	F3 = risoluzione a 800x600
	F4 = risoluzione a 1024x768
	F5 = passa da 16 a 32 bits

	UP = movimento in avanti
	DOWN = movimento indietro
	W = movimento in alto
	S = movimento in basso
	LEFT = movimento a sinistra
	RIGHT = movimento a destra

	A = rotazione asse Y
	D = rotazione asse Y

	MOUSE = rotazioni sugli assi

	X = attiva / disattiva il frustum
	O = attiva / disattiva l'oscillazione della visuale durante il movimento
	L = accende / spegne la luce (problemi con la luce spenta)
	N = attiva / disattiva la neve (pioggia e neve non funzionano insieme)
	Z = disattiva il rendering dei particle engine
	P = attiva / disattiva la pioggia (pioggia e neve non funzionano insieme)
	B = abilita / disabilita le trasparenze
	F = attiva / disattiva la nebbia
	J = attiva / disattiva la caduta della pioggia e della neve (non serve a niente ma mi piaceva)
	T = attiva / disattiva gli alberi
	K = attiva / disattiva la visualizzazione del cielo
	G = cambia la mappa da cui è generato il terreno e non disegna tutta la scena


Ver 0.32:
- movimenti basati sul tempo e non sul frame rate (movimenti uguali su tutti i computer)
- rotazione degli oggetti basata sul tempo e non sul frame rate
- visualizzazione degli FPS nella finestra e non sulla barra del titolo...finalmente :)	(e lampeggia anche)
- funzione per calcolare il frame rate spostata nel main
- aggiunta funzione per la la visualizzazione delle informazioni
- risoluzione visualizzata sempre su schermo e non sulla barra del titolo di win
- impossibile scendere sotto il livello del suolo
- possibilità di bloccare la caduta della pioggia e della neve con J
- riscritte alcuni parti del codice

Ver 0.31:
- aggiunti particle engine anche se limitati
- aggiunta la possibilità di caricare particle engine dal file di testo
- particle engine disabilitati premendo Z
- aggiunta nuovo tipo di poligoni (quadrati animati)
- animazioni e velocità delle animazioni specificate nel file di testo del mondo
- gli oggetti 3D vengono disegnati con una precisioni proporzionale alla distanza
- possibilità di disabilitare il frustum culling con X
- alcune informazioni vengono visualizzate sullo schermo
- risolto problema con l'inizializzazione della pioggia
- in modalità fullscreen viene visualizzata la risoluzione

Ver 0.30:
- vengono disegnati solo i poligoni visibili (problemi con triangoli e quindi anche quandrati)
- pioggia
- modificato tutto il sistema di caricamento e di visualizzazione dei poligoni (sembra poco ma è una cosa che mi ha ucciso)
- eliminazione delle display list per i poligoni normali
- i poligoni vengono disegnati con un solo ciclo for comune per tutti

Ver 0.29:
- nuova classe per la camera e il movimento basata sui vettori (camera gestita come oggetto)
- utilizzo della funzione gluLookAt per la camera
- risolti problemi durante gli spostamenti con oscillazione
- risolto problema quando si passava da 16 a 32 bits con F5 (prima non cambiava)
- in modalità finestra viene visualizzata la risoluzione

Ver 0.28:
- programmazione ad oggetti anche per texture e terreno
	- il terreno è diventato un oggetto e non più una parte del main
	- le texture vengono gestite da un oggetto (Texture_Manager)

Ver 0.27:
- neve (con la neve cambiano le texture del cielo e del terreno)
- oggetti in 3D formati da punti che cambiano forma (morphing)
- risolto un problema nella visualizzazione degli oggetti 3D con morphing
- gli oggetti vengono visualizzati anche di giorno

Ver 0.26b:
- modificato il filmato iniziale (fatto con vari tipi di spostamento)
- risolto il problema nel passaggio a fullscreen e nel cambio di risoluzione
- file per il caricamento delle texture inseriti nel main.cpp
- file per leggere il .RAW della displacement map inserito in Genesis_Terrain.cpp
- possibilità di cambiare oltre alla risoluzione anche la profondità del colore (F5)

Ver 0.26:
- filmato iniziale di presentazione o attivato dal tasto E
- scritta lampeggiante durante il filmato
- risolto problema con l'illuminazione del terreno
- gli alberi vengono sempre visualizzati
- tolte variabili inutili nel translatef delle sfere

Ver 0.25:
- OTTIMIZZAZIONI
	- memorizzazione del terreno in una lista di visualizzazione
	- memorizzazione dei triangoli in un'unica lista di visualizzazione
	- memorizzazione degli alberi in un'unica lista di visualizzazione (migliora le prestazioni con molti alberi)
	- migliorata la lista di visualizzazione dei cilindri
- possibilità di visualizzare un terreno mosso
- aggiunta stanza sud
- cielo infinito sugli assi X e Z (impossibile uscire dal cielo senza volare
- cielo ingrandito con raggio 70
- quando il terreno non è piatto viene rimpicciolito per aumentare il dettaglio
- migliorata la visualizzazione delle texture sul terreno

Ver 0.24:
- terreno caricato da una displacement map (DA MIGLIORARE)
- migliorato l'effetto nebbia perchè il terreno viene caricato con + poligoni
- spostamento in un file separato delle funzioni di caricamento texture
- file per il caricamento e il rendering del terreno
- vengono visualizzate vicino al frame rate anche le coordinate della posizione

Ver 0.23:
- supporto per il mouse (rotazioni sugli assi X e Y)
- aggiunta stanza est
- migliorata la funzione per la generazione casuale degli alberi
- corretto problema quando veniva premuto il tasto L la prima volta

Ver 0.22:
- visualizzazione testo sullo schermo
- risolto il problema con la luce (che non sempre si spegneva e dava problemi alle trasparenze)
- generazione casuali di alberi (i tipi di albero sono memorizzati in liste di visualizzazione)
- numero di alberi da disegnare scelto nel file .TXT
- cambia il colore della nebbia con la luce
- cancellazione liste di visualizzazione prima di uscire (compresa quella del testo)
- corretto il sistema di caricamento delle texture (ora vengono caricate tutte insieme e non con una chiamata x texture

Ver 0.21:
- migliorata la nebbia anche se non ancora perfetta
- aggiunto il cielo e il terreno
- aggiunta delle liste di visualizzazione per cielo e terreno
- il terreno viene disegnato in modo diverso in caso di nebbia
- aggiunta stanza ovest