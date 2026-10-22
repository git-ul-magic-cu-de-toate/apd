package barber;

public class Client extends Thread {
    private final int id;

    public Client(int id) {
        super();
        this.id = id;
    }

    @Override
    public void run() {
        try {
            Main.modifySeats.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (Main.chairs > 0) {
            // client occupies a seat
            Main.chairs--;

            System.out.println("Client " + id + " is waiting for haircut");
            System.out.println("Available seats: " + Main.chairs);

            // client is ready to get a haircut (not yee yee ass haircut like Franklin)
            Main.clientReady.release();

            // modified the number of available chairs
            Main.modifySeats.release();

            try {
                // waiting for the barber to get ready
                Main.barberReady.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Client " + id + " is served by the barber");

            Main.leftClients[id] = Main.SERVED_CLIENT;
        } else {
            Main.modifySeats.release();
            System.out.println("Client " + id + " left unserved");
            Main.leftClients[id] = Main.UNSERVED_CLIENT;
        }
    }
}
