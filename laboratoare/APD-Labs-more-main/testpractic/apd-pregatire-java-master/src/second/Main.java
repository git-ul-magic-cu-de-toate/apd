package second;

import java.util.Arrays;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        if (args.length < 2) {
            System.err.println("Insufficient arguments");
            System.exit(-1);
        }

        int size = Integer.parseInt(args[0]);
        int numberOfThreads = Integer.parseInt(args[1]);

        Thread[] threads = new Thread[numberOfThreads];
        int totalSum = 0;

        int[] array = new int[size - 1];
        int count = 1;
        for (int i = 0; i < size - 1; i++) {
            array[i] = count;
            // sum += (i + 1)
            count++;
            if (count == size / 2) {
                count++;
            }
        }

        System.out.println(Arrays.toString(array));

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Task(i, array, size - 1, numberOfThreads);
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].start();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].join();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            totalSum += ((Task) threads[i]).getSum();
        }

        int expectedSum = (size * (size + 1)) / 2;
        System.out.println("Missing number = " + (expectedSum - totalSum));
    }
}
