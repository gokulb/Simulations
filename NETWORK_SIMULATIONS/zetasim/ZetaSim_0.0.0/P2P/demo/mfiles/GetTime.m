function time = GetTime(tline)
    lb = find( tline == '[');
    rb = find( tline == ']');
    stp = rb(1) + 2;
    %tline(stp:end)
    tmp = find( tline(stp:end) == ' ');
    edp = tmp(1) + (stp - 1 ) - 2;
    time = str2num(tline(stp:edp));
end