package barSem;

public class MyThread extends Thread {
    private final int id;
    private final BarrierWithSemaphores barrier;

    public MyThread(int id, BarrierWithSemaphores barrier) {
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
