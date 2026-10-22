package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @author Gabriel Gutu <gabriel.gutu at upb.ro>
 */
public class Buffer {
    private final Queue<Integer> queue;
    public static Semaphore consumerSemaphore;
    public static Semaphore producerSemaphore;

    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        consumerSemaphore = new Semaphore(0);
        producerSemaphore = new Semaphore(size);
    }

    public void put(int value) {
        try {
            producerSemaphore.acquire();
        } catch (InterruptedException ex) {
            Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
        }

        synchronized (queue) {
            queue.add(value);
        }

        consumerSemaphore.release();

    }

    public int get() {
        try {
            consumerSemaphore.acquire();
        } catch (InterruptedException ex) {
            Logger.getLogger(Buffer.class.getName()).log(Level.SEVERE, null, ex);
        }

        int a = -1;
        synchronized (queue) {
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
        }

        producerSemaphore.release();
        return a;
    }
}
