function indices=lengths_to_indices(lengths)

indices=cumsum(lengths(:));
indices=[[0; indices(1:length(indices)-1)]+1,indices];
