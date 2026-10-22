package atomicUsage;

import java.util.concurrent.atomic.AtomicInteger;

public class Main {

    static  AtomicInteger myInteger = new AtomicInteger(0);
    public static void main(String[] args) {
        int NUMBER_OF_THREADS = 4;

        Thread[] t = new Thread[NUMBER_OF_THREADS];

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            t[i] = new Task(i);
            t[i].start();
        }

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println(myInteger.get());
    }


}
