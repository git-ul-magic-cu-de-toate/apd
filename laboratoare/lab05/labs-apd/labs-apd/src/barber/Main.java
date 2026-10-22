package barber;

import java.util.concurrent.Semaphore;

public class Main {
    public final static int TOTAL_CHAIRS = 3;
    public final static int TOTAL_CLIENTS = 7;

    public final static int SERVED_CLIENT = 1;
    public final static int UNSERVED_CLIENT = 2;

    public static int[] leftClients = new int[TOTAL_CLIENTS];

    public static final Semaphore barberReady = new Semaphore(0);
    public static final Semaphore clientReady = new Semaphore(0);
    public static final Semaphore modifySeats = new Semaphore(1);

    public static int clients = TOTAL_CLIENTS;
    public static int chairs = TOTAL_CHAIRS;

    public static void main(String[] args) throws InterruptedException {
        Thread barberThread = new Barber();
        Thread[] clientThreads = new Client[clients];

        for (int i = 0; i < clients; i++) {
            clientThreads[i] = new Client(i);
        }

        barberThread.start();
        for (Thread clientThread : clientThreads) {
            clientThread.start();
            Thread.sleep(100);
        }

        barberThread.join();
        for (var thread: clientThreads) {
            thread.join();
        }

        int unservedClients = 0;
        for (var client: leftClients) {
            if (client == UNSERVED_CLIENT) {
                unservedClients++;
            }
        }

        System.out.println("There were " + unservedClients + " unserved clients");
    }
}
