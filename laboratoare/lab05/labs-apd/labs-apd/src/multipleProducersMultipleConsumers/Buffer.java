package multipleProducersMultipleConsumers;

import java.util.logging.Level;
import java.util.logging.Logger;

public class Buffer {
    int a = -1;

    public void put(int value) {
        synchronized (this) {
            while (a != -1) {
                try {
                    // wait for a consumer to consume
                    this.wait();
                } catch (InterruptedException ex) {
                    Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            a = value;
            // notify all consumers
            this.notifyAll();
        }
    }

    public int get() {
        synchronized (this) {
            while (a == -1) {
                try {
                    // wait for a producer to produce
                    this.wait();
                } catch (InterruptedException ex) {
                    Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            int aux = a;
            a = -1;
            // notify all producers
            this.notifyAll();
            return aux;
        }
    }
}
