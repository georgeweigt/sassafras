A = c(6,0,2,8,11,4,13,1,8,0)
B = c(0,2,3,1,18,4,14,9,1,9)
C = c(13,10,18,5,23,12,5,16,1,20)

print(t.test(A,B,var.equal=TRUE))
print(t.test(A,C,var.equal=TRUE))
print(t.test(B,C,var.equal=TRUE))

YA = c(6,0,2,8,11,4,13,1,8,0)
YB = c(0,2,3,1,18,4,14,9,1,9)
sse = var(YA) * (length(YA) - 1) + var(YB) * (length(YB) - 1)
dfe = length(YA) + length(YB) - 2
mse = sse / dfe
se = sqrt(mse * (1 / length(YA) + 1 / length(YB)))
t = (mean(YA) - mean(YB)) / se
mean(YA) - mean(YB) - qt(0.975,dfe) * se
mean(YA) - mean(YB) + qt(0.975,dfe) * se

print(mean(YA) - mean(YB) - qt(0.975,dfe) * se)
print(mean(YA) - mean(YB) + qt(0.975,dfe) * se)

print(2 * (1 - pt(t,dfe)))

print(t.test(A,B,var.equal=TRUE))
