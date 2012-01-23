function totalSize = GetLoad(fileName, BSSet)
%bootServer = 0;
fid = fopen(fileName);
type = {};
time = [];
src = [];
des = [];
pktName = {};
pktSize = [];

while ~feof(fid)
    tline = fgetl(fid);
    typde = {type, GetPara(tline, 1)};
    time = [time, str2num(GetPara(tline, 2))];
    src = [src, str2num(GetPara(tline, 3))];
    des = [des, str2num(GetPara(tline, 4))];
    pktName = {pktName, GetPara(tline, 5)};
    pktSize = [pktSize, str2num(GetPara(tline, 6))];
end
%[type, time, src, des, pktName, pktSize]=textread('out.tr','%s%f%d%d%s%d', -1, 'headerlines',4);
otherNodeIdx = 1:length(src);
for bootServer = BSSet
    idx = find(src == bootServer);
    otherNodeIdx = setdiff(otherNodeIdx, idx);
    totalSize = sum(pktSize(idx));
    if totalSize > 1e6
	disp(['total size in boot server ', num2str(bootServer),' is:', num2str(totalSize / 1e6), 'MB'])
	continue
    end
    if totalSize > 1e3
	disp(['total size in boot server ', num2str(bootServer), ' is:', num2str(totalSize / 1e3), 'KB'])
	continue
    end
    disp(['total size in boot server ', num2str(bootServer), ' is:', num2str(totalSize), 'B'])
end
%totalSize
totalSize = sum(pktSize(otherNodeIdx));
if totalSize > 1e6
    disp(['total size in other nodes is:', num2str(totalSize / 1e6), 'MB'])
    return
end
if totalSize > 1e3
    disp(['total size in other nodes is:', num2str(totalSize / 1e3), 'KB'])
    return
end
disp(['total size in other nodes is:', num2str(totalSize), 'B'])
end

end



function para = GetPara(tline, idx)
    pos = find( isspace(tline) );
    if idx == 1
	stp = 1;
	edp = pos(1) - 1;
	para = tline(stp:edp);
    elseif idx == length(pos) + 1
	stp = pos(end) + 1;
	para = tline(stp:end);
    else
	stp = pos(idx-1) + 1;
	edp = pos(idx) - 1;
	para = tline(stp:edp);
    end
end
