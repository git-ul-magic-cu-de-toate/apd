package task7;

import util.BinarySearchTreeNode;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class SearchValue<T extends Comparable<T>> implements Runnable {
    private final ExecutorService tpe;
    private final BinarySearchTreeNode<T> tree;
    private final T value;
    private final AtomicInteger counter;
    private final CompletableFuture<T> completableFuture;

    public SearchValue(
            ExecutorService tpe,
            BinarySearchTreeNode<T> tree,
            T value,
            AtomicInteger counter, CompletableFuture<T> completableFuture
    ) {
        this.tpe = tpe;
        this.tree = tree;
        this.value = value;
        this.counter = counter;
        this.completableFuture = completableFuture;
    }

    @Override
    public void run() {
        if (tree != null) {
            if (value.equals(tree.getValue())) {
                completableFuture.complete(tree.getValue());
                tpe.shutdown();
            } else if (tree.getValue().compareTo(value) > 0) {
                counter.incrementAndGet();
                Runnable t = new SearchValue<>(tpe, tree.getLeft(), value, counter, completableFuture);
                tpe.submit(t);
            } else {
                counter.incrementAndGet();
                Runnable t = new SearchValue<>(tpe, tree.getRight(), value, counter, completableFuture);
                tpe.submit(t);
            }
        }

        int left = counter.decrementAndGet();
        if (left == 0) {
            completableFuture.complete(null);
            tpe.shutdown();
        }
    }
}
