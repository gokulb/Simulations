function node = GetNode(tline)
    lb = find( tline == '[');
    rb = find( tline == ']');
    stp = lb(2)+ 1;
    edp = rb(2) - 1;
    node = str2num( tline(stp:edp));
end