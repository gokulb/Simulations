function fileTag = GetFile(tline)
    lb = find( tline == '[');
    rb = find( tline == ']');
    stp = lb(3)+ 1;
    edp = rb(3) - 1;
    fileTag = str2num( tline(stp:edp));
end