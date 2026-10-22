package task8;

import util.BinarySearchTreeNode;

import java.util.concurrent.RecursiveTask;

public class MaximumHeightCalculator<T extends Comparable<T>> extends RecursiveTask<Integer> {
    private final BinarySearchTreeNode<T> tree;

    public MaximumHeightCalculator(BinarySearchTreeNode<T> tree) {
        this.tree = tree;
    }

    @Override
    protected Integer compute() {
        if (tree == null) {
            return 0;
        }

        MaximumHeightCalculator<T> left = new MaximumHeightCalculator<>(tree.getLeft());
        MaximumHeightCalculator<T> right = new MaximumHeightCalculator<>(tree.getRight());

        left.fork();
        right.fork();

        return 1 + Math.max(left.join(), right.join());
    }
}
