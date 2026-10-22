
Tema #2 APD - Planificarea de Task-uri Într-un Datacenter

Nume: Gîrniță Alexandra-Claudia
Grupa: 332CC

	Aceasta tema a constat in implementarea a două clase - MyHost și MyDispatcher - în Java, cu scopul de a simula planificarea eficientă a task-urilor într-un datacenter.

1. Clasa MyHost
	MyHost reprezintă un nod individual în sistemul de planificare a task-urilor. Principalul său rol este de a gestiona și procesa task-uri, având la bază o coadă de priorități:
	-> PriorityBlockingQueue: Folosită pentru stocarea task-urilor, asigură dacă task-urile sunt ordonate în funcție de prioritate și, în caz de egalitate, în funcție de timpul lor de start.
	-> Metoda run(): Aceasta este locul in care se planifica task-urile, mai exact sunt așteptate și procesate. Dacă coada este goală, firul de execuție așteaptă până când un nou task este adăugat.
	-> Metoda shouldPreempt(): Verifică dacă un task cu prioritate mai mare a sosit, caz în care task-ul curent este preemptat.
	-> Metoda processCurrentTask(): Simulează execuția task-ului curent prin apelul metodei sleep pentru durata specificată a task-ului și apoi îl marchează ca finalizat.
	-> Metoda addTask(): Adaugă un nou task în coada de task-uri myQueue, iar operația este sincronizată pe un obiect lock pentru a preveni conflictele între thread-uri
	-> Metoda getQueueSize(): Calculează și returnează suma duratelor rămase ale tuturor task-urilor din coadă, iar operația este si ea deasemenea sincronizată
	-> Metoda shutdown(): Se ocupă cu oprirea prelucrării task-urilor cu ajutorul unei variabile booleene isRunning ce se transformă în false

2. Clasa MyDispatcher
	MyDispatcher funcționează ca un dispatcher centralizat, redistribuind task-urile către diferite instanțe Host:
		-> Round Robin (RR): Distribuie task-urile uniform între toate nodurile
		-> Shortest Queue (SQ): Alocă task-uri nodului cu cea mai scurtă coadă
		-> Size Interval Task Assignment (SITA): Distribuie task-uri bazându-se pe dimensiunea lor către noduri specifice
		-> Least Work Left (LWL): Trimite task-uri către nodul cu cel mai mic volum de muncă rămas