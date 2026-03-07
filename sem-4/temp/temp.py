# Definition for singly-linked list.
class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next
class Solution:
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
        a = self.conv(l1)
        b = self.conv(l2)
        print(a,b)
        s = a+b
        t = 0
        while s>0:
            t = t*10 + s%10
            s = s//10
        return self.mk(t)

    def mk(self,n):
        l=ListNode()
        while n>=0:
            l = ListNode(n%10, l.next)
            
        return l

    def conv(self,l):
        a=0
        while l.next != None:
            a+=l.val
            l=l.next
        return a


