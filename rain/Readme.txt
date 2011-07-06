Versione	0.20

- Comandi
	ESC = esce dal programma
	F1 = passaggio da modalità window a fullscreen
	F2 = risoluzione a 640x480
	F3 = risoluzione a 800x600
	F4 = risoluzione a 1024x768
	F5 = passa da 16 a 32 bits

	UP = movimento in avanti
	DOWN = rallenta il movimento in avanti
	LEFT = spostamento a sinistra
	RIGHT = spostamento a destra

	CTRL = spara

	SPAZIO = mette in pausa il gioco

	K = diminuisce il livello di dettaglio
	L = aumenta il livello di dettaglio
	D = cambia la difficoltà di gioco
	T = cambia il terreno


Ver 0.20:
- aggiunti altri nemici + mostro di fine livello :)
- aggiunta una variabile con il tempo di gioco che viene usata per gestire l'arrivo dei nemici
- i power_up danno punti se non possono aumentare le caratteristiche

Ver 0.18:
- aggiunti power_up (con una loro classe)
- aggiunte le luci alla scena (un po' di realismo, ma cmq da migliorare)
- nuovi nemici (cattivi)
- le particelle e i triangoli delle esplosioni vengono disegnati solo se hanno life > 0 (prima c'era un errore)
- 4 tipi di power_up (numero colpi, potenza colpi, energia, punti)
- aggiunti cerchi che ruotano intorno ai power_up
- aggiunto codice nela classe Rain_Fire x avere + inclinazioni del colpo

Ver 0.17:
- aggiunti i frammenti all'esplosione del nemico
- ottimizzata parte del codice (molt al posto di div, puntatore a struttura invece che i singoli parametri in Add_XPL)
- aggiunto movimento alle esplosioni (centro di massa)
- aggiunte esplosioni circolari anche se non sono bellissime :(
- il numero di particelle delle esplosioni dipende dalla grandezza del nemico
- durante la pausa non scompaiono le esplosioni e lo scudo resta fermo

Ver 0.16:
- non si può uscire nell'angolo in basso (anche se vorrei migliorare il sitema usato)
- reimplementato il sistema di controllo dei nemici, dei colpi e dei colpi nemici per vedere se sono nello schermo
- grazie alla riga sopra non è + possibile colpire i nemici fuori dallo schermo :)
- è possibile sparare in continuazione tenendo premuto CTRL
- funzione ottimizzata per gestire le collisioni di ship separata da quella per le distanze tra altri oggetti
- il generatore dei numeri casuali è davvero casuale xkè inizializzato con l'orologio
- l'inclinazione della nave è moltiplicata per Frame_Interval
(  2295 righe di codice :)  )

Ver 0.15:
- i nemici sparano nella direzione dell'astronave
- si possono colpire i colpi nemici (che formano delle piccole esplosioni)
- migliorata la classe di gestione delle esplosioni, ora si possono modificare la grandezza delle particelle e il loro numero
- è possibile modificare in parte il livello di dettaglio
- due livelli di difficoltà

Ver 0.13:
- nemici che vengono generati casualmente
- energia che diminuisce con la collisione con i nemici
- punteggio
- migliorate le esplosioni
- ogni nemico ha un colore e le esplosioni sono di quel colore (un po' + rosse)

Ver 0.12:
- aggiunta classe per la gestione dei nemici
- aggiunta classe per la gestione delle esplosioni
- sistemata collisione tra astronave e nemici
- sistemata collisione tra colpi e nemici
- i nemici espodono :)

Ver 0.11:
- aggiunta la possibilità di sparare
- aggiunto il fuoco (anche se non perfetto) dietro l'astronave
- due fuochi invece di uno
- aggiunto lo sfondo stellato (anche se devo aggiungere alcune cose)
- aggiunti semplici oggetti sul terreno verde
- risolto problema di sfasamento con il fuoco dell'astronave quando inclinata

Ver 0.1:
- funzione reale che calcola la distanza e servirà nelle collisioni
- compila :)