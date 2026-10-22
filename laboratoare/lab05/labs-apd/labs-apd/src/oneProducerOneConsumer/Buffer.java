/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package oneProducerOneConsumer;

import java.util.logging.Level;
import java.util.logging.Logger;

public class Buffer {
    private int a = -1;

    public void put(int value) {
        synchronized (this) {
            if (a != -1) {
                try {
                    // wait for the consumer to consume
                    this.wait();
                } catch (InterruptedException ex) {
                    Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            a = value;
            System.out.println("Produced " + a);
            // notify the consumer
            this.notify();
        }
    }

    public int get() {
        synchronized (this) {
            if (a == -1) {
                try {
                    // wait for the producer to produce
                    this.wait();
                } catch (InterruptedException ex) {
                    Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            int aux = a;
            a = -1;
            System.out.println("Consumed " + aux);
            // notify the producer
            this.notify();
            return aux;
        }
    }
}
