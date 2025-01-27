import java.awt.Color;
import java.awt.Container;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.geom.Rectangle2D;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.Scanner;
import java.util.StringTokenizer;

import javax.swing.JFrame;
import javax.swing.JPanel;

public class FloorplanImager extends JPanel {
	private static final long serialVersionUID = 1L;
	List<Rectangle> blocks;
	TreeNode root;
	
	public FloorplanImager(String inputFile) {
		// Read in the file and create tree
		read(inputFile);
		// Figure out coords for each module
		assignXY();
	}

	private void assignXY() {
		int height = (int)calculateHeight(root);
		// Bottom left element in the tree is at the bottom left of the floorplan
		assignX(root, 0);
		assignY(root, height);
	}

	private int assignY(TreeNode node, int currentY) {
		if(!node.V && !node.H) {
			// If we are at a module give it the currentY value
			blocks.get(node.index).y = currentY - blocks.get(node.index).height;
			// Height of this subtree is module height
			return blocks.get(node.index).height;
		} else if(node.V) {
			// Both subtrees are at thi Y value for V
			int newHeight1 = assignY(node.left, currentY);
			int newHeight2 = assignY(node.right, currentY);
			int maxHeight = (newHeight1 > newHeight2) ? newHeight1 : newHeight2;
			// Height of V subtree is max of children
			return maxHeight;
		}
		// Left tree gets this Y
		int leftHeight = assignY(node.left, currentY);
		// Right tree gets Y above left tree
		int rightHeight = assignY(node.right, currentY - leftHeight);
		// Height of H subtree is sum of children
		return leftHeight + rightHeight;
	}

	private int assignX(TreeNode node, int currentX) {
		if(!node.V && !node.H) {
			// If we are at a module give it the currentX value
			blocks.get(node.index).x = currentX;
			// Width of this subtree is module width
			return blocks.get(node.index).width;
		} else if(node.H) {
			// Both subtrees are at this X value for H
			int newWidth1 = assignX(node.left, currentX);
			int newWidth2 = assignX(node.right, currentX);
			int maxWidth = (newWidth1 > newWidth2) ? newWidth1 : newWidth2;
			// Width of H subtree is max of children
			return maxWidth;
		}
		// Left tree gets this X
		int leftWidth = assignX(node.left, currentX);
		// Right tree gets X at end of left tree
		int rightWidth = assignX(node.right, currentX + leftWidth);
		// Width of V subtree is sum of children
		return leftWidth + rightWidth;
	}

	double calculateHeight(TreeNode node) {
		// If we are at a terminal return the height
		if(!node.V && !node.H) {
			return blocks.get(node.index).height;
		}
		double height1, height2;
		height1 = calculateHeight(node.left);
		height2 = calculateHeight(node.right);
		// For a horizontal cut sum the heights
		if(node.H) {
			return height1 + height2;
		}
		// For a vertical cut take the max
		return (height1 > height2) ? height1 : height2;
	}
	
	private void generateTree(String expression) {
		// Tokenize and build tree
		StringTokenizer st = new StringTokenizer(expression, "-");
		List<String> tokens = new ArrayList<String>();
		while(st.hasMoreTokens()) {
			tokens.add(st.nextToken());
		}
		root = new TreeNode();
		// Flip the string so we can iterate forwards through it
		Collections.reverse(tokens);
		createSubTree(root, tokens);
	}

	private void createSubTree(TreeNode node, List<String> tokens) {
		String id = tokens.get(0);
		if("V".equals(id) || "H".equals(id)) {
			node.V = "V".equals(id);
			node.H = "H".equals(id);
			node.right = new TreeNode();
			createSubTree(node.right, tokens.subList(1, tokens.size()));
			node.left = new TreeNode();
			// Use for counting the size of the first subtree
			int size = 1;
			int index = 0;
			do {
				index++;
				if("V".equals(tokens.get(index)) || "H".equals(tokens.get(index))) {
					// Add for operators
					size++;
				} else {
					// Subtract for terminals
					size--;
				}
			} while(size > 0);
			createSubTree(node.left, tokens.subList(index+1, tokens.size()));
		} else {
			node.index = Integer.parseInt(id) - 1;
		}
	}

	private void read(String inputFile) {
		// Pull in the file and create the array of modules
		Scanner scanner;
		try {
			scanner = new Scanner(new FileInputStream(inputFile));
			String expression = scanner.nextLine();
			blocks = new ArrayList<Rectangle>();
			scanner.nextLine();
			while(scanner.hasNext()) {
				int width = scanner.nextInt();
				int height = scanner.nextInt();
				blocks.add(new Rectangle(width, height));
			}
			generateTree(expression);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	private class TreeNode {
		boolean H = false, V = false;
		int index;
		TreeNode left, right;
	}

	public void paint(Graphics g) {
		Graphics2D g2 = (Graphics2D)g;

		// Draw each module with a random color and a black border
		Random gen = new Random();
		for(Rectangle2D rect : blocks) {
			g2.setColor(new Color(gen.nextInt(256), gen.nextInt(256), gen.nextInt(256)));
			g2.fill(rect);
			g2.setColor(Color.black);
			g2.draw(rect);
		}
	}

	public static void main(String[] args) {
		JFrame frame = new JFrame();
		frame.setTitle("Floorplan");
		frame.setSize(1000, 500);
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
		});
		Container contentPane = frame.getContentPane();
		contentPane.add(new FloorplanImager("output.flr"));

		frame.setVisible(true);
	}
}
