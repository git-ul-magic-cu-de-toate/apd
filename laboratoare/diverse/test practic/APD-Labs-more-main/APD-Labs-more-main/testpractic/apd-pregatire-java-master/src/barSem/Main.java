package barSem;

public class Main {
    private static final int NO_THREADS = 8;

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[NO_THREADS];
        BarrierWithSemaphores barrier = new BarrierWithSemaphores(NO_THREADS);

        for (int i = 0; i < NO_THREADS; i++) {
            threads[i] = new MyThread(i, barrier);
        }

        for (int i = 0; i < NO_THREADS; i++) {
            threads[i].start();
        }

        for (int i = 0; i < NO_THREADS; i++) {
            threads[i].join();
        }
    }
}
