package bug3;

/**
 * Why is value set correct even though we use different locks in
 * different threads?
 */
public class MyThread implements Runnable {
    private static final Object lock = new Object();
    public static int value = 0;

    @Override
    public void run() {
        synchronized (lock) {
            for (int i = 0; i < Main.N; i++) {
                value = value + 3;
            }
        }
    }
}
