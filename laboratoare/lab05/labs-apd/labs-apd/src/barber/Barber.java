package barber;

public class Barber extends Thread {
    @Override
    public void run() {
        int servedClients = 0;

        do {
            try {
                // waiting for a client to cut his / her hair
                Main.clientReady.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            try {
                // client is being served, let's increase the number of available chairs
                Main.modifySeats.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            Main.chairs++;

            // incremented the number of available chairs
            Main.modifySeats.release();

            Main.barberReady.release();

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Barber served client");
            ++servedClients;

        } while (checkIfThereAnyClients());
    }

    private boolean checkIfThereAnyClients() {
        int count = 0;
        for (var client: Main.leftClients) {
            if (client == 0) {
                count++;
            }
        }

        return count != 0;
    }
}
