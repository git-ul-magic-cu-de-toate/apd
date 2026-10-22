package semaphore;

import philosophersProblem.Philosopher;

import java.util.concurrent.Semaphore;

public class Main {
    static Semaphore mySemaphore = new Semaphore(-1);

    public static void main(String[] args) {

        mySemaphore.release();
        mySemaphore.release();
        mySemaphore.release();
        System.out.println(mySemaphore.availablePermits());
        try {
            mySemaphore.acquire();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        System.out.println(mySemaphore.availablePermits());
        System.out.println("am ajuns");

    }

}