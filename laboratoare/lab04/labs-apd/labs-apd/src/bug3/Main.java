package bug3;

/**
 * DO NOT MODIFY
 */
public class Main {
    public static final int N = 100000;
    public static int N_ITERATIONS = 100;
    private static final int NO_THREADS = 2;

    public static void main(String[] args) {
        Thread[] threads = new Thread[2];
        boolean sw = true;
        for (int j = 0; j < N_ITERATIONS; j++) {
            MyThread.value = 0;
            for (int i = 0; i < NO_THREADS; i++) {
                threads[i] = new Thread(new MyThread());
                threads[i].start();
            }
            for (int i = 0; i < NO_THREADS; i++) {
                try {
                    threads[i].join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            if (MyThread.value != 2 * 3 * N) {
                System.out.println("i was different than " + 2 * 3 * N + ", it is " + MyThread.value);
                sw = false;
            }
        }
        if (sw) {
            System.out.println("Something is strange");
        }
    }
}
