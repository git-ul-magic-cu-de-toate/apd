package reentrantBarSem;

import java.util.concurrent.Semaphore;

public class ReentrantBarrierWithSemaphores {
    private static class Wrapper {
        private int count;

        public Wrapper(int count) {
            this.count = count;
        }

        public void incrementCount() {
            count++;
        }

        public int getCount() {
            return count;
        }

        public void reset() {
            count = 0;
        }
    }

    private final int numberOfThreads;
    private final Wrapper countThreads1 = new Wrapper(0);
    private final Wrapper countThreads2 = new Wrapper(0);
    private final Semaphore semaphore1 = new Semaphore(0);
    private final Semaphore semaphore2 = new Semaphore(0);

    public ReentrantBarrierWithSemaphores(int numberOfThreads) {
        this.numberOfThreads = numberOfThreads;
    }

    public void await() throws InterruptedException {
        this.phase(semaphore1, countThreads1);
        this.phase(semaphore2, countThreads2);
    }

    private void phase(Semaphore semaphore, Wrapper countThreads) throws InterruptedException {
        synchronized (this) {
            countThreads.incrementCount();
        }

        if (countThreads.getCount() == numberOfThreads) {
            for (int i = 0; i < numberOfThreads; i++) {
                semaphore.release();
            }
            countThreads.reset();
        }

        semaphore.acquire();
    }
}
