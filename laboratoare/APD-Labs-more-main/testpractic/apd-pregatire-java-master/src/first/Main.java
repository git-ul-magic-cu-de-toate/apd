package first;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        if (args.length < 2) {
            System.err.println("Insufficient arguments");
            System.exit(-1);
        }

        int size = Integer.parseInt(args[0]);
        int numberOfThreads = Integer.parseInt(args[1]);

        Thread[] threads = new Thread[numberOfThreads];
        int[] partialMins = new int[numberOfThreads];
        int[] partialMaxs = new int[numberOfThreads];

        int[] array = new int[size];
        for (int i = 0; i < size; i++) {
            array[i] = size - i + 5;
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Task(i, array, size, numberOfThreads);
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].start();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].join();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            partialMaxs[i] = ((Task) threads[i]).getMaximum();
            partialMins[i] = ((Task) threads[i]).getMinimum();
        }

        int globalMax = partialMaxs[0];
        int globalMin = partialMins[0];

        for (int i = 0; i < numberOfThreads; i++) {
            if (globalMax <= partialMaxs[i]) {
                globalMax = partialMaxs[i];
            }

            if (globalMin >= partialMins[i]) {
                globalMin = partialMins[i];
            }
        }

        System.out.println("Maximum = " + globalMax);
        System.out.println("Minimum = " + globalMin);

    }
}
