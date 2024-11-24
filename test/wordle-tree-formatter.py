def format_decision_tree(input_file, output_file):
    """
    Format a decision tree by filling empty start with previous line content
    
    Args:
        input_file (str): Name of the input file
        output_file (str): Name of the output file
    """
    # Read from file
    with open(input_file, 'r') as f:
        lines = [line.rstrip().upper() for line in f if line.strip()]
    
    # Process each line
    formatted_lines = []
    prev_line = None
    
    for line in lines:
        # If this is the first line, just add it
        if prev_line is None:
            formatted_lines.append(line)
            prev_line = line
            continue
        
        # Find where the non-space content starts in current line
        content_start = len(line) - len(line.lstrip())
        
        # Create new line by combining previous line's prefix with current line's content
        new_line = prev_line[:content_start] + line[content_start:]
        
        formatted_lines.append(new_line)
        prev_line = new_line
    
    # Write to output file
    with open(output_file, 'w') as f:
        f.write('\n'.join(formatted_lines))

# Process the tree
format_decision_tree('tree.txt', 'tree_plus.txt')