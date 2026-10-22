package doubleVectorElements;

public class VectorTask implements Runnable {

    private final int start;
    private final int end;

    public VectorTask(int id) {
        start = (int) (id * (double) Main.v.length / Main.NUMBER_OF_THREADS);
        end = Math.min((int) ((id + 1) * (double) Main.v.length / Main.NUMBER_OF_THREADS), Main.v.length);
    }

    public void run() {
        for (int i = start; i < end; ++i) {
            Main.v[i] *= 2;
        }
    }

}