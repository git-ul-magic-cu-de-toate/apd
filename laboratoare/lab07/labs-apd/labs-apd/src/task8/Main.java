package task8;

import util.BSTOperations;
import util.BinarySearchTreeNode;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;

public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        BinarySearchTreeNode<Integer> binarySearchTree = new BinarySearchTreeNode<>(3);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 6);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 9);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 2);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 10);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 1);

        ForkJoinPool forkJoinPool = new ForkJoinPool();
        MaximumHeightCalculator<Integer> calculator = new MaximumHeightCalculator<>(binarySearchTree);
        forkJoinPool.execute(calculator);
        System.out.println(calculator.get());
    }
}
