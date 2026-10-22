package doubleVectorElements;

public class Main {

    public static int[] v;
    public static int NUMBER_OF_THREADS = 4;

    public static void main(String[] args) {
        int N = 100000013;
        v = new int[N];

        for (int i = 0; i < N; i++)
            v[i] = i;

        // Parallelize me
        //for (int i = 0; i < N; i++) {
        //	v[i] = v[i] * 2;
        //}

        Thread[] t = new Thread[NUMBER_OF_THREADS];

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            t[i] = new Thread(new VectorTask(i));
            t[i].start();
        }

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }

}
