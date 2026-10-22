package first;

public class Task extends Thread {
    private final int id;
    private final int[] array;
    private final int size;
    private final int threads;
    private int maximum;
    private int minimum;

    public Task(int id, int[] array, int size, int threads) {
        this.id = id;
        this.array = array;
        this.size = size;
        this.threads = threads;
    }

    @Override
    public void run() {
        int start = id * (int) Math.ceil((double) size / (double) threads);
        int end = Math.min((id + 1) * (int) Math.ceil((double) size / (double) threads), size);

        maximum = array[start];
        minimum = array[start];

        for (int i = start; i < end; i++) {
            if (maximum <= array[i]) {
                maximum = array[i];
            }

            if (minimum >= array[i]) {
                minimum = array[i];
            }
        }
    }

    public int getMaximum() {
        return maximum;
    }

    public int getMinimum() {
        return minimum;
    }
}
