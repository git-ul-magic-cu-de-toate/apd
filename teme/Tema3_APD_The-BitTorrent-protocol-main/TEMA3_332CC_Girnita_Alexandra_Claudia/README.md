# tema3

Girnita Alexandra-Claudia
332CC

Aceasta tema implementeaza transferul de date simulat al protocolului BitTorent.

Primul pas este initializarea clientilor cu datele din fisiere. Avand in vedere ca un client lucreaza cu 2 threaduri, datele care necesita partajarea intre cele doua sunt declarate global.

Ca schema de stocare a datelor am ales definirea a 3 structuri:
FileData care retine toate informatiile necesare despre un fisier, cum ari fi numele, segmentele si swarm-ul clientilor care detin segmente din fisier. 
Segment retine hash-ul segmentului si un flag care identifica daca clientul il detine sau nu.
Swarm_unit stocheaza informatiile unui seed/peer pentru fisier si anume rank-ul fisierului si un array de dimensiunea numarului de segmente care contin flagul 1 pentru segmentele detinute.

Initial se citesc datele despre fisier detinute in array-ul client_files dupa care se aloca spatiu si pentru fisierele care urmeaza sa fie citite. Dupa initializarea clientilor, fiecare incepe transmisia de date cu traker-ul prin thread-ul de download. Se transmit trakerului totate fisierele detinute si in acelasi timp trakerul construieste si swarm-ul pentru fiecare.

In etapa de download se cauta primul segment lipsa dintr-un fisier dat, daca nu se gaseste nici-un segment lipsa, este scris fisierul de output pentru acel fisier si se trece la urmatorul fisier incrementand indexul array-ului de fisiere descarcate. Se alege random un clientul care detine segmentul si il cere. El transmite mai intai numele fisierului si dupa indexul segmentului ca seed-ul sa stie ce sa trimita inapoi. Intre timp thread-ul de update asteapta cereri de segmente pana cand primeste un mesaj de stop se la tracker. Dupa 10 segmente descarcate se face update-ul datelor catre tracker exact la fel ca datele initiale, folosind send_data_to_tracker. Tracker-ul primeste datele si doar actualizeaza swarmurile. In continuare clientii trebuie ceara noile swarm-uri folosind metoda request_swarm_update(rank) da din cauza ei uneori programul se blocheaza de aceea ea este comentata pentru ca nu s-a identificat probmea blocarii dupa primirea noilor liste, cel mai probabil depinzand de alegerea seed-ului intr-un anumit moment. Cand numarul de fisiere care trebuie descarcate devine 0, clientul termina descarcarea si trimite mesaj-ul catre tracker apoi inchide thread-ul de download. Cand tracker-ul a primit mesaje de la toti clientii trimite mesaje thread-urilor de update sa se opreasca si se termina programul. 