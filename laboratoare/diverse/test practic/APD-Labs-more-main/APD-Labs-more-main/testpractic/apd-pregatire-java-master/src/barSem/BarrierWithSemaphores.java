package barSem;

import java.util.concurrent.Semaphore;

public class BarrierWithSemaphores {
    private final int numberOfThreads;
    private int countThreads = 0;
    private final Semaphore semaphore = new Semaphore(0);

    public BarrierWithSemaphores(int numberOfThreads) {
        this.numberOfThreads = numberOfThreads;
    }

    public void await() throws InterruptedException {
        synchronized (this) {
            countThreads++;
        }

        if (countThreads == numberOfThreads) {
            // semaphore.release(numberOfThreads);

            for (int i = 0; i < numberOfThreads; i++) {
                semaphore.release();
            }
        }

        semaphore.acquire();
    }
}
