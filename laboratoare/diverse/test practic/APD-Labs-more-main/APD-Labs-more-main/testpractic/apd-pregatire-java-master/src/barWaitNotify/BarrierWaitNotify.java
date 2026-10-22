package barWaitNotify;

public class BarrierWaitNotify {
    private final int numberOfThreads;
    private int countThreads = 0;

    public BarrierWaitNotify(int numberOfThreads) {
        this.numberOfThreads = numberOfThreads;
    }

    public synchronized void await() throws InterruptedException {
        countThreads++;
        if (countThreads == numberOfThreads) {
            this.notifyAll();
        } else {
            this.wait();
        }
    }

    /*
    // aceeasi chestie ca mai sus
    public void await() throws InterruptedException {
        synchronized (this) {
            countThreads++;
            if (countThreads == numberOfThreads) {
                this.notifyAll();
            } else {
                this.wait();
            }
        }
    }
     */
}
