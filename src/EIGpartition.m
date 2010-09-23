function [part1, part2, cut] = EIGpartition(A, S, type)
% Takes in weighted adjacency matrix A, size constraint S (in form [3,5] 
% for 3-5 or 4-4 partition of size 8 set and type of 'ratiocut' or
% 'cutsize'
%
% Returns indices to the nodes to be put in each partition and the
% appropriate cutsize or ratiocut
 
n = size(A,1);

% Degree matrix
D = diag(sum(A));

% Laplacian matrix
Q = D - A;

% Get all eigenvalues and their vectors
[vecs, vals] = eig(Q);

% Sort the second eigenvalue's vector
% Z is index vector and will define optimal node ordering
[sorted, Z] = sort(vecs(:,2));

% Compute cut size and ratiocut for relevant partitions
cutsize = sum(sum(A(Z((S(1)+1):n),Z(1:S(1)))))/2;
ratiocut = cutsize/(S(1)*(n-S(1)));
cutsize_at = S(1);
ratiocut_at = S(1);
for i = (S(1)+1):S(2)
    new_cutsize = sum(sum(A(Z((i+1):n),Z(1:i))))/2;
    if (new_cutsize < cutsize)
        cutsize = new_cutsize;
        cutsize_at = i;
    end
    new_ratiocut = new_cutsize/(i*(n-i));
    if (new_ratiocut < ratiocut)
        ratiocut = new_ratiocut;
        ratiocut_at = i;
    end
end
cut = ratiocut;
cut_at = ratiocut_at;
if(strcmp(type,'cutsize'))
    cut = cutsize;
    cut_at = cutsize_at;
end

part1 = Z(1:cut_at);
part2 = Z(cut_at+1:n);