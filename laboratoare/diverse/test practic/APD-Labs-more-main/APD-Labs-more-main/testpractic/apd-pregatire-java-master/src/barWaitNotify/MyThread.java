package barWaitNotify;

public class MyThread extends Thread {
    private final int id;
    private final BarrierWaitNotify barrier;

    public MyThread(int id, BarrierWaitNotify barrier) {
        this.id = id;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        System.out.println("Before barrier " + id);
        try {
            barrier.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("After barrier " + id);
    }
}
